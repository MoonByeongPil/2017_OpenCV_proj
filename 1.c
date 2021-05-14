#include <cv.h>
#include <highgui.h>
#include <math.h>

void ucvViewCam(IplImage*image, CvCapture* capture, char *WindowName);
void ucvViewImage(char *WindowName, IplImage *image);

int main()
{
	unsigned char R,B,G,D;	
	
	int x,y;
	int i,c=128;
	int index;	
		
	int maxR=0,maxG=0,maxB=0;	
	
	unsigned char MR,MB,MG;
	
	int bins = 256;
	int sizes[] = {bins};
	float xranges[] = {0, 256};
	float*ranges[] = {xranges};
	
	IplImage*image,*sampleR,*sampleG,*sampleB,*sample,*result;
		
	CvHistogram*ShistR = cvCreateHist( 1, sizes, CV_HIST_SPARSE, ranges, 1),
			   *ShistB = cvCreateHist( 1, sizes, CV_HIST_SPARSE, ranges, 1),
			   *ShistG = cvCreateHist( 1, sizes, CV_HIST_SPARSE, ranges, 1);
			   
	CvCapture* capture = cvCaptureFromCAM(0);						
	
	image = cvRetrieveFrame( capture,1 );
	
	result=cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 3);
	
	sample=cvLoadImage("p-3_tamplet.jpg",-1);
	sampleR=cvCreateImage(cvSize(sample->width, sample->height), IPL_DEPTH_8U, 1);
	sampleB=cvCreateImage(cvSize(sample->width, sample->height), IPL_DEPTH_8U, 1);
	sampleG=cvCreateImage(cvSize(sample->width, sample->height), IPL_DEPTH_8U, 1);
	
	cvSplit(sample,sampleB,sampleR,sampleG,NULL);

	cvCalcHist(&sampleR,ShistR,0,0);
	cvCalcHist(&sampleG,ShistG,0,0);
	cvCalcHist(&sampleB,ShistB,0,0);


	for(i=0;i<254;i++) if(cvGetReal1D( ShistR->bins,i )>cvGetReal1D( ShistR->bins,maxR )) maxR=i; 
	for(i=0;i<254;i++) if(cvGetReal1D( ShistG->bins,i )>cvGetReal1D( ShistG->bins,maxG )) maxG=i;
	for(i=0;i<254;i++) if(cvGetReal1D( ShistB->bins,i )>cvGetReal1D( ShistB->bins,maxB )) maxB=i; 
	
	cvNamedWindow( "camera",0);

	printf("%d,%d,%d",maxR,maxG,maxB);
	
	while(1)
	{
		ucvViewCam(image,capture,"camera");
			
		for(y = 0; y < (image->height); y+=4) 
		{			
			for(x = 0; x < (image->widthStep); x += 4) 
			{
				index = y * image->widthStep + x*3;
				
				R = image->imageData[index+2];	
				G = image->imageData[index+1];	
				B = image->imageData[index+0];	
	
				if((abs(maxR-R)>c)||(abs(maxB-B)>c)||(abs(maxG-G)>c))
				{
					result->imageData[index+2] = 0;	
					result->imageData[index+1] = 0;
					result->imageData[index+0] = 0;
				}
				else
				{
					MR= c-abs(maxR-R);
					MB= c-abs(maxB-B);
					MG= c-abs(maxG-G);

	
					D=(MR+MG+MB)/3*2;
					result->imageData[index+2] = D;	
					result->imageData[index+1] = D;
					result->imageData[index+0] = D;
				}
			}	
		}
		
		ucvViewImage("result",result);
		
		if( cvWaitKey(10) >= 0 )
            break;
	}
    cvReleaseCapture( &capture );					//이미지 릴리스 
	cvReleaseImage( &image );						
    cvDestroyAllWindows();
	
}

void ucvViewCam(IplImage*image, CvCapture* capture, char *WindowName)
{
	if( capture )										//캡쳐값이 존재할경우 
	{    					
        image = cvRetrieveFrame( capture,1 );		//image에 capture에 받아온 단일 프레임을 저장 
		cvShowImage( WindowName, image );				//image를 camera창에 표시  		     
    }
}

void ucvViewImage(char *WindowName, IplImage *image)
{
	cvNamedWindow( WindowName, 0);
	cvShowImage( WindowName, image );
}

