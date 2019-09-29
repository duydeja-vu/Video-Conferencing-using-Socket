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
    int fd_server;
    struct sockaddr_in addr_serv, addr_client;
    int addr_len = sizeof(addr_serv);

    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(PORT_NO);
    addr_serv.sin_addr.s_addr = INADDR_ANY;

    int size_text_mess = 128;
    char *text_mess;
    text_mess = (char *)malloc(size_text_mess * sizeof(char));

    fd_server = socket(AF_INET, SOCK_STREAM, 0);

    if (fd_server < 0)
    {
        printf("CREAT SOCKET FAILED!\n");
    }
    else
    {
        printf("CREAT SOCKET SUCCESSFUL\n");
    }

    if (0 == bind(fd_server, (struct sockaddr *)&addr_serv, sizeof(addr_serv)))
    {
        printf("BIND SUCCESSFULL\n");
    }
    else
    {
        printf("BIND FAILED\n");
        exit(0);
    }
    printf("LISTENING\n");

    listen(fd_server, 5);

    int new_socket;
    new_socket = accept(fd_server, (struct sockaddr *)&addr_serv, &addr_len);

    read(new_socket, text_mess, size_text_mess);
    printf("Client Request %s\n", text_mess);

    fflush(stdin);

    gets(text_mess);
    send(new_socket, text_mess, size_text_mess, sendrecvflag);
    if (0 != strcmp(text_mess, "Yes"))
    {
        printf("Exception\n");
        return 1;
    }
    else
    {
        printf("VIDEO CALL");
    }
    int *pixel_arr_recv, *pixel_arr_send;
    int frame_width = 848, frame_height = 480;
    int size_image_mess;
    size_image_mess = frame_width * frame_height * sizeof(int);
    pixel_arr_recv = (int *)malloc(size_image_mess);
    pixel_arr_send = (int *)malloc(size_image_mess);

    while (1)
    {

        recv(new_socket, pixel_arr_recv, size_image_mess, 0);
        CvScalar pixel;
        int pixel_value;
        IplImage img_receive = *cvCreateImage(
            cvSize(frame_width, frame_height), IPL_DEPTH_8U, 1);
        for (int i = 0; i < frame_height; i++)
        {
            for (int j = 0; j < frame_width; j++)
            {
                pixel_value = *(pixel_arr_recv + i * frame_width + j);
                pixel.val[0] = pixel_value;
                cvSet2D(&img_receive, i, j, pixel);
            }
        }

        IplImage frame, gray_frame;
        frame = *cvLoadImage("test5.jpg", CV_LOAD_IMAGE_UNCHANGED);
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
        send(new_socket, pixel_arr_send, size_image_mess, 0);
        cvShowImage("Duy", &img_receive);
        cvWaitKey(1);
    }
    close(fd_server);
}