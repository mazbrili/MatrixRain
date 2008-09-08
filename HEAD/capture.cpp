//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	capture.cpp
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <unistd.h>
#include <fcntl.h>              // low-level i/o
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#include "capture.h"
#include "stuff.h"
//--------------------------------------------------------------
#define CLEAR(x) memset (&(x), 0, sizeof (x))
//--------------------------------------------------------------
int xioctl(int fd, int request, void *arg)
{
        int r;

        do r = ioctl (fd, request, arg);
        while (-1 == r && EINTR == errno);

        return r;
}

Capture::MMapBuffer::MMapBuffer(int fd, size_t len, off_t off):length(len)
{
	start = mmap (NULL, length, PROT_READ | PROT_WRITE /* required */, MAP_SHARED /* recommended */, fd, off);
	if (MAP_FAILED == start)
		throw runtime_error("mmap failed");
}
Capture::MMapBuffer::~MMapBuffer()
{
	if (-1 == munmap (start, length))
		throw runtime_error("munmap failed");
}

Capture::Capture(unsigned int covet_w, unsigned int covet_h, const char* dev_name): buffer(NULL), buffers(NULL), decoders(NULL)
{
	struct stat st={0};
        if (-1 == stat (dev_name, &st))
	{
		throw runtime_error("'%s' is not exist", dev_name);
        }

        if (!S_ISCHR (st.st_mode))
	{
		throw runtime_error("'%s' is not a character device", dev_name);
        }
//		throw runtime_error("'%s' is not a character device", dev_name);

        device = open (dev_name, O_RDWR | O_NONBLOCK, 0);

        if (-1 == device)
	{
                throw runtime_error("can not open '%s'", dev_name);
        }

	device_capcabilities(dev_name);

	//major device number of 81
        struct v4l2_capability cap;

        if (-1 == xioctl (device, VIDIOC_QUERYCAP, &cap))
	{
                if (EINVAL == errno)
		{
			throw runtime_error("'%s' is not V4L2 evice", dev_name);
                } else
		{
                        throw runtime_error("VIDIOC_QUERYCAP");
                }
        }

        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		throw runtime_error("'%s' is no video capture device", dev_name);
        }

	if (!(cap.capabilities & V4L2_CAP_STREAMING))
	{
		throw runtime_error("'%s' does not support streaming i/o", dev_name);
	}


        // Select video input, video standard and tune here.
	
	struct v4l2_cropcap cropcap;
	unsigned int min;


	CLEAR (cropcap);

        cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (0 == xioctl (device, VIDIOC_CROPCAP, &cropcap))
	{
	/*	struct v4l2_crop crop;
		CLEAR(crop);
                crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//V4L2_BUF_TYPE_VBI_CAPTURE;
		

		v4l2_rect rect={0,0,width, height};
                crop.c = rect;//cropcap.defrect; // reset to default 

                if (-1 == xioctl (device, VIDIOC_S_CROP, &crop))
		{
                        switch (errno) {
                        case EINVAL:
			//	errno_exit("xioctrl ! VIDIOC_S_CROP");
                                // Cropping not supported. 
                                break;
                        default:
				errno_exit("default error ?");
                                // Errors ignored. 
                                break;
                        }
                }
*/        } else {	
		throw runtime_error("ioctrl VIDIOC_CROPCAP error ");
                // Errors ignored. 
        }
	
	v4l2_format fmt;
        CLEAR (fmt);

        fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = covet_w; 
        fmt.fmt.pix.height      = covet_h;
        fmt.fmt.pix.pixelformat = select_format();

        if (-1 == xioctl (device, VIDIOC_S_FMT, &fmt) )
	{
                throw runtime_error("VIDIOC_S_FMT");
	}
	else
	{
		fprintf(stderr,"w:%i h:%i sizeimage:%i\n",
						fmt.fmt.pix.width, 
						fmt.fmt.pix.height,
						fmt.fmt.pix.sizeimage);
		
		captured_width = fmt.fmt.pix.width;
		captured_height= fmt.fmt.pix.height;

		for(unsigned int i=0; i<sizeof(supported_formats)/sizeof(supported_formats[0]); i++)
		{
			if(fmt.fmt.pix.pixelformat == supported_formats[i].fourcc)
			{
				decoders = supported_formats[i].decoder;
				break;
			}
		}
	}
        // Note VIDIOC_S_FMT may change width and height.

	// Buggy driver paranoia.
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;


	struct v4l2_requestbuffers req;
        CLEAR (req);

        req.count	= 8;
        req.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory	= V4L2_MEMORY_MMAP;

	if (-1 == xioctl (device, VIDIOC_REQBUFS, &req))
	{
		if (EINVAL == errno)
		{
			runtime_error("'%s' does not support memory mapping\n", dev_name);
                }
		else
		{
                        throw runtime_error("VIDIOC_REQBUFS");
                }
        }

        if (req.count < 2)
	{
		runtime_error("Insufficient buffer memory on %s\n", dev_name);
        }
	num_buffers = req.count;
	buffers = new MMapBuffer*[num_buffers];

        for (unsigned int i=0; i<num_buffers; ++i)
	{
                struct v4l2_buffer buf;
                CLEAR (buf);

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = i;

                if (-1 == xioctl (device, VIDIOC_QUERYBUF, &buf))
                        throw runtime_error("VIDIOC_QUERYBUF");

		buffers[i] = new MMapBuffer(device, buf.length, buf.m.offset);

        	if (-1 == xioctl (device, VIDIOC_QBUF, &buf))
                    	throw runtime_error("VIDIOC_QBUF");

        }

	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl (device, VIDIOC_STREAMON, &type))
		throw runtime_error("VIDIOC_STREAMON");
}

Capture::~Capture()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl (device, VIDIOC_STREAMOFF, &type))
		throw runtime_error("VIDIOC_STREAMOFF");


	for (unsigned int i=0; i<num_buffers; ++i)
		delete buffers[i];

	delete[] buffers;

	if (-1 == close (device))
	{
	        throw runtime_error("close");
	}
        device = 0;
}

const char* Capture::operator()()
{
	fd_set fds;

	FD_ZERO (&fds);
	FD_SET (device, &fds);

	timeval timeout={0, 50000};

	int r = select (device+1, &fds, NULL, NULL, &timeout);

	if (-1 == r)
	{
		if (EINTR == errno)
		return NULL;
	}

	if (0 == r)
	{
	//	fprintf (stderr, "select timeout\n");
		return NULL;
	}

	struct v4l2_buffer buf;
	CLEAR (buf);

    	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl (device, VIDIOC_DQBUF, &buf))
	{
		fprintf(stderr,"call VIDIOC_DQBUF\n");
		switch (errno)
		{
			case EAGAIN:
			return NULL;

			case EIO:
			// Could ignore EIO, see spec.

			// fall through

			default:
			throw std::runtime_error("VIDIOC_DQBUF");
		}
	}
	assert (buf.index < num_buffers);

	if (-1 == xioctl (device, VIDIOC_QBUF, &buf))
		throw std::runtime_error("VIDIOC_QBUF");

	if( buffer )
	{
		MMapBuffer* current = buffers[buf.index];		
		decoders[format]((char*)current->start, (char*)current->start + current->length, buffer);
	}
	return buffer;
}

void Capture::device_capcabilities(const char* dev_name)
{
	v4l2_capability cap;

        if (-1 == xioctl (device, VIDIOC_QUERYCAP, &cap))
	{
                if (EINVAL == errno)
		{
                        fprintf (stderr, " is no V4L2 device\n");
                        exit (EXIT_FAILURE);
                } else
		{
                        throw std::runtime_error("VIDIOC_QUERYCAP");
                }
        }
	/*
	//__u8	driver[16];	// i.e. "bttv" 
	//__u8	card[32];	// i.e. "Hauppauge WinTV"
	//__u8	bus_info[32];	// "PCI:" + pci_name(pci_dev)
	//__u32   version;      // should use KERNEL_VERSION()
	//__u32	capabilities;	// Device capabilities
	//	__u32	reserved[4];*/

	fprintf(stdout,"device:\t\t%s [%s]\n", dev_name, cap.card);
	fprintf(stdout,"driver:\t\t%s\n", cap.driver);
	fprintf(stdout,"bus info:\t%s\n", cap.bus_info);
	char major = (cap.version>>16) & 0xFF;
	char minor = (cap.version>>8) & 0xFF;
	char subminor = (cap.version) & 0xFF;
	fprintf(stdout,"version:\t%d.%d.%d\n", major, minor, subminor);
	fprintf(stdout,"capabilities:\n");	

	if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)	fprintf(stdout,"\tIs a video capture device\n");
	if(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)	fprintf(stdout,"\tIs a video output device\n");
	if(cap.capabilities & V4L2_CAP_VIDEO_OVERLAY)	fprintf(stdout,"\tCan do video overlay\n");
	if(cap.capabilities & V4L2_CAP_VBI_CAPTURE)	fprintf(stdout,"\tIs a raw VBI capture device\n");
	if(cap.capabilities & V4L2_CAP_VBI_OUTPUT)	fprintf(stdout,"\tIs a raw VBI output device\n");
	if(cap.capabilities & V4L2_CAP_SLICED_VBI_CAPTURE)  fprintf(stdout,"\tIs a sliced VBI capture device\n");
	if(cap.capabilities & V4L2_CAP_SLICED_VBI_OUTPUT)   fprintf(stdout,"\tIs a sliced VBI output device\n");
	if(cap.capabilities & V4L2_CAP_RDS_CAPTURE)	fprintf(stdout,"\tRDS data capture\n");
	if(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_OVERLAY)fprintf(stdout,"\tCan do video output overlay\n");

	if(cap.capabilities & V4L2_CAP_TUNER) fprintf(stdout,"\tHas a tuner\n");
	if(cap.capabilities & V4L2_CAP_AUDIO) fprintf(stdout,"\tHas audio support\n");
	if(cap.capabilities & V4L2_CAP_RADIO) fprintf(stdout,"\tIs a radio device\n");

	if(cap.capabilities & V4L2_CAP_READWRITE) fprintf(stdout,"\tRead/write systemcalls\n");
	if(cap.capabilities & V4L2_CAP_ASYNCIO)	  fprintf(stdout,"\tAsync I/O\n");
	if(cap.capabilities & V4L2_CAP_STREAMING) fprintf(stdout,"\tStreaming I/O ioctls\n");
}


unsigned int Capture::select_format()
{
	fprintf(stderr,"video formats:\n");
	errno = 0;

	unsigned int format = 0;
	for(unsigned int i=0; errno != EINVAL && i<16; i++)
	{
		v4l2_fmtdesc desc={0};
		desc.index = i;
		desc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
		if (-1 != xioctl(device, VIDIOC_ENUM_FMT, &desc))
		{
			if(0U == format)	// find if not found
			{
				for(unsigned int j=0; j<sizeof(supported_formats)/sizeof(supported_formats[0]); j++)			
				{
					if(supported_formats[j].fourcc == desc.pixelformat )
					{
						format = desc.pixelformat;
						break;
					}
				}
			}
			char str[5]={'\0'};
			str[0] = (desc.pixelformat>>0 ) & 0xFF;
			str[1] = (desc.pixelformat>>8 ) & 0xFF;
			str[2] = (desc.pixelformat>>16) & 0xFF;
			str[3] = (desc.pixelformat>>24) & 0xFF;
			
			if(format != desc.pixelformat)
				fprintf(stderr,"\tFourCC[%s] %s\n", str, (const char*)desc.description);
			else
				fprintf(stderr,"select->FourCC[%s] %s\n", str, (const char*)desc.description);
		}
	}

	if(0 == format) fprintf(stderr,"not found supported formats, sorry\n");
	return format;
}

const Capture::Converter Capture::supported_formats[4]=
{
	{ V4L2_PIX_FMT_YUYV, { YUYVtoRGB, YUYVtoBGR, YUYVtoGRAY } },
	{ V4L2_PIX_FMT_UYVY, { UYVYtoRGB, UYVYtoBGR, UYVYtoGRAY } },
	{ V4L2_PIX_FMT_GREY, { 0,	  0,	   GREYtoGREY } },
	{ V4L2_PIX_FMT_YUV422P, { 0, 	0,	   YUV422PtoGREY } },
};


void Capture::GREYtoGREY(const char* src, const char* end, char* dst)
{
	memcpy(dst, src, end - src);
}

void Capture::YUYVtoRGB(const char* src, const char* end, char* dst)
{
	
/*	rgb[0] = float(y) +1.403f*float(v);
	rgb[1] = float(y) -0.344f*float(u) -0.714f*float(v);
	rgb[2] = float(y) + 1.770f*float(u);
*/
/*	rgb[0] = 1.164f * float(float(y)-16) + 2.018f*float(float(u)-128);
	rgb[1] = 1.164f * float(float(y)-16) - 0.813f*float(float(v) - 128) - 0.391f*float(float(u) - 128);
	rgb[2] = 1.164f * float(float(y)-16) + 1.596f*float(float(v) - 128);
*/
	for(; src<end; src+=4) 
	{
		char y1 = src[0];
		char u  = src[1];
		char y2 = src[2];
		char v  = src[3];

		float r = + 2.032f*float(u);
		float g = - 0.395f*float(u) -0.581f*float(v);
		float b = + 1.140f*float(v);

		dst[0] = float(y1) + r;			
		dst[1] = float(y1) + g;
		dst[2] = float(y1) + b;

		dst +=3;

		dst[0] = float(y2) + r;			
		dst[1] = float(y2) + g;
		dst[2] = float(y2) + b;

		dst +=3;
	}
}

void Capture::YUYVtoBGR(const char* src, const char* end, char* dst)
{
	for(; src<end; src+=4) 
	{
		char y1 = src[0];
		char u  = src[1];
		char y2 = src[2];
		char v  = src[3];

		float r = + 2.032f*float(u);
		float g = - 0.395f*float(u) -0.581f*float(v);
		float b = + 1.140f*float(v);

		dst[0] = float(y1) + b;			
		dst[1] = float(y1) + g;
		dst[2] = float(y1) + r;

		dst +=3;

		dst[0] = float(y2) + b;			
		dst[1] = float(y2) + g;
		dst[2] = float(y2) + r;

		dst +=3;
	}
}

void Capture::YUYVtoGRAY(const char* src, const char* end, char* dst)
{
	for(; src<end; src+=4) 
	{
		*dst++ = src[0];
		*dst++ = src[2];
	}
}

void Capture::UYVYtoRGB(const char* src, const char* end, char* dst)
{
	for(; src<end; src+=4) 
	{
		char u  = src[0];
		char y1 = src[1];
		char v  = src[2];
		char y2 = src[3];

		float r = + 2.032f*float(u);
		float g = - 0.395f*float(u) -0.581f*float(v);
		float b = + 1.140f*float(v);

		dst[0] = float(y1) + r;			
		dst[1] = float(y1) + g;
		dst[2] = float(y1) + b;

		dst +=3;

		dst[0] = float(y2) + r;			
		dst[1] = float(y2) + g;
		dst[2] = float(y2) + b;

		dst +=3;
	}
}

void Capture::UYVYtoBGR(const char* src, const char* end, char* dst)
{
	for(; src<end; src+=4) 
	{
		char u  = src[0];
		char y1 = src[1];
		char v  = src[2];
		char y2 = src[3];

		float r = + 2.032f*float(u);
		float g = - 0.395f*float(u) -0.581f*float(v);
		float b = + 1.140f*float(v);

		dst[0] = float(y1) + b;			
		dst[1] = float(y1) + g;
		dst[2] = float(y1) + r;

		dst +=3;

		dst[0] = float(y2) + b;			
		dst[1] = float(y2) + g;
		dst[2] = float(y2) + r;

		dst +=3;
	}
}

void Capture::UYVYtoGRAY(const char* src, const char* end, char* dst)
{
	for(; src<end; src+=4) 
	{
		*dst++ = src[1];
		*dst++ = src[3];
	}
}

void Capture::YUV422PtoGREY(const char* src, const char* end, char* dst)
{
	memcpy(dst, src, end - src);
}
//--------------------------------------------------------------

