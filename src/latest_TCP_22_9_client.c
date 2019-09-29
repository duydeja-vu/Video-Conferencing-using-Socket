#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <opencv/cv.h>

#define IP_PROTOCOL 0
#define IP_ADDRESS "127.0.0.1"
#define PORT_NO 15050
#define sendrecvflag 0

int main()
{
    int fd_client;
    struct sockaddr_in addr_serv, addr_client;
    int addr_len = sizeof(addr_serv);

    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(PORT_NO);
    addr_serv.sin_addr.s_addr = inet_addr("127.0.0.1");

    int size_text_mess = 128;
    char *text_mess;
    text_mess = (char *)malloc(size_text_mess * sizeof(char));

    fd_client = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_client < 0)
    {
        printf("CREAT SOCKET FAILED!\n");
    }
    else
    {
        printf("CREAT SOCKET SUCCESSFUL\n");
    }

    if (connect(fd_client, (struct sockaddr *)&addr_serv, sizeof(addr_serv)) < 0)
    {
        printf("Connect Failed\n");
    }
    else
    {
        printf("ACCEPT");
    }
    printf("Start session\n");
    char *temp = "Do you want to video call";
    strcpy(text_mess, temp);
    send(fd_client, text_mess, size_text_mess, sendrecvflag);
    printf("-----SENT REQUEST-----\n");
    read(fd_client, text_mess, size_text_mess);
    printf("\nSERVER REPLY: %s\n", text_mess);
    if (0 != strcmp(text_mess, "Yes"))
    {
        printf("SERVER REJECT VIDEO CALL REQUEST\n");
        exit(0);
    }
    else
    {
        CvCapture *capture;
        IplImage cam_frame, gray_frame;
        int *pixel_arr_send, *pixel_arr_recv;
        int frame_width, frame_height;
        int size_image_mess;
        capture = cvCreateCameraCapture(-1);
        if (!capture)
        {
            printf("WEBCAM NOT FOUND\n");
            exit(1);
        }

        frame_width = 848;
        frame_height = 480;
        size_image_mess = frame_width * frame_height * sizeof(int);
        pixel_arr_send = (int *)malloc(size_image_mess);
        pixel_arr_recv = (int *)malloc(size_image_mess);

        int count = 0;

        while (1)
        {
            cam_frame = *cvQueryFrame(capture);
            if (!&cam_frame)
            {
                printf("FRAME ERR");
                exit(0);
            }
            cvSaveImage("test1.jpg", &cam_frame, 0);
            IplImage frame;
            frame = *cvLoadImage("test1.jpg", CV_LOAD_IMAGE_UNCHANGED);
            gray_frame = *cvCreateImage(cvSize(frame_width, frame_height), IPL_DEPTH_8U, 1);
            cvCvtColor(&frame, &gray_frame, CV_BGR2GRAY);

            CvScalar pixel_send;
            int pixel_send_value;

            for (int i = 0; i < frame_height; i++)
            {
                for (int j = 0; j < frame_width; j++)
                {
                    pixel_send = cvGet2D(&frame, i, j);
                    pixel_send_value = pixel_send.val[0];
                    *(pixel_arr_send + i * frame_width + j) = pixel_send_value;
                }
            }
            send(fd_client, pixel_arr_send, size_image_mess, 0);

            recv(fd_client, pixel_arr_recv, size_image_mess, 0);
            CvScalar pixel_recv;
            int pixel_recv_value;
            IplImage img_receive = *cvCreateImage(
                cvSize(frame_width, frame_height), IPL_DEPTH_8U, 1);
            for (int i = 0; i < frame_height; i++)
            {
                for (int j = 0; j < frame_width; j++)
                {
                    pixel_recv_value = *(pixel_arr_recv + i * frame_width + j);
                    pixel_recv.val[0] = pixel_recv_value;
                    cvSet2D(&img_receive, i, j, pixel_recv);
                }
            }
            cvShowImage("Huy", &img_receive);
            cvWaitKey(1);
        }
        cvReleaseCapture(&capture);
        cvDestroyAllWindows();
        close(fd_client);
    }
}
