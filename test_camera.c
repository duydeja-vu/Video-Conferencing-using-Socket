#include<opencv2/core/core_c.h>
#include<opencv2/highgui/highgui_c.h>
#include<stdio.h>


int main()
{
    cvNamedWindow("Window", CV_WINDOW_AUTOSIZE);
    CvCapture* capture = cvCreateCameraCapture(-1);
    if(!capture)
    {
        printf("err");
    }
    else
    {
        cvNamedWindow("windows", CV_WINDOW_AUTOSIZE);
        while (1)
        {
            IplImage* frame = cvQueryFrame(capture);
            if(!frame)
            {
                printf("Frame err");
                break;
            }
            IplImage n = *frame;
            int m = n.nSize;
            cvShowImage("windows", frame);
            printf("%d\n", m);
            cvWaitKey(1);
            

        }
        cvReleaseCapture(&capture);
        cvDestroyAllWindows();
        
    }
    return 0;
    

}
