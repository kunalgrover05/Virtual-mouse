#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<cv.h>
#include <iostream>
#include <conio.h>
#include<windows.h>

using namespace cv;
using namespace std;

#define STEP 20
#define MAX_CHANGE 30

int ScrWidth,ScrHeight;

void get_screen_size()															//Get screen resolution
{
	ScrWidth = GetSystemMetrics(SM_CXSCREEN);
	ScrHeight = GetSystemMetrics(SM_CYSCREEN);
}

void set_cursor(int pos[],int x,int y) {                                        // set cursor according to position 
	int status = SetCursorPos(ScrWidth - pos[0]*ScrWidth/x,(pos[1]*ScrHeight)/y);  // taking care of lateral inversion
	if(status == 0) {
		cout<<"Unable to set cursor";
	}
}                             

int main()
{
	int top[2],x,y;

    get_screen_size();
    
	CvCapture* captRefrnc = cvCaptureFromCAM(CV_CAP_ANY);
	namedWindow( "Display window", CV_WINDOW_AUTOSIZE );
	if ( !captRefrnc)
	{
		cout  << "Could not open reference " << endl;
		cvWaitKey(0);
		return -1;
	}

	///////////// Allocating memory for images  /////////////////////// 
	IplImage* imageptr = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);
	IplImage* imagenew = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);
	IplImage* imagenew1 = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);
	IplImage* imagenew2 = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);

	//////////////Get initial background and save it////////////////////
	cout<<"Press any key to calibrate";
	cvWaitKey(0);
	imagenew = cvQueryFrame(captRefrnc);
	cvCvtColor(imagenew,imageptr,CV_BGR2GRAY);
	//Mat imgbg(imageptr,true);
	x=imageptr->width;
	y=imageptr->height;
	cout<<"\nCalibrated background\nPress any key to exit";

	while(1)
	{
		imagenew = cvQueryFrame(captRefrnc);
		cvCvtColor(imagenew,imagenew1,CV_BGR2GRAY);

		///Background subtraction and thresholding the difference of the two images
		cvAbsDiff(imagenew1,imageptr,imagenew2);                        
		cvThreshold(imagenew2,imagenew2,30,255,CV_THRESH_BINARY);
     
        /////Set top[y] as maximum y ie lowermost point on image
      	top[1] = y;

        ///Divide the image into smaller squares of STEP*STEP and find the topmost & leftmost square with average change more than MAX_CHANGE  
		for(int j=0; j<y; j += STEP) 
		{
            for(int i=0; i<x; i += STEP)
			{
				cvSetImageROI(imagenew2,cvRect(i,j,STEP,STEP));
				cvRectangle(imageptr,cvPoint(i,j),cvPoint((i+STEP),(j+STEP)),cvScalar(0,255,0));
				CvScalar average = cvAvg(imagenew2);
				cvResetImageROI(imagenew2);
				if(average.val[0] > MAX_CHANGE) 
				{
                    if(j < top[1]) 
                    {
						top[0] = i + STEP/2;
						top[1] = j + STEP/2;
					}
				}
			}
		}

        ////////////Set cursor and make a circle around the cursor/////////////////
		cvCircle(imagenew,cvPoint(top[0],top[1]),STEP,cvScalar(0,100,100),5);
		set_cursor(top,x,y);

		cvShowImage("Display window", imagenew);
        if(waitKey(10)>=0)
            {
                break;
            }
 
	}
    return 1;
    
}