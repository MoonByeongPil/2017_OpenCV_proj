#include <cv.h>
#include <highgui.h>
#include <stdio.h>


void ucvViewCam(IplImage*image, CvCapture* capture, char *WindowName);

int main()
{
	unsigned char R,B,G,D;	
	
	int x,y,z;
	int index,save;	
	int loop=5;
	int bins = 256;
	int sizes[] = {bins};
		
	float max_value = 0, min_value = 0;	
	
	float MR,MB,MG,M;
	
	float xranges[] = {0, 255};
	float*ranges[] = {xranges};
	
	IplImage*image,*imageR,*imageG,*imageB,*sampleR,*sampleG,*sampleB,*sample,*result;
		
	CvHistogram*ShistR = cvCreateHist( 1, sizes, CV_HIST_ARRAY, ranges, 1),
			   *ShistB = cvCreateHist( 1, sizes, CV_HIST_ARRAY, ranges, 1),
			   *ShistG = cvCreateHist( 1, sizes, CV_HIST_ARRAY, ranges, 1),
			   *IhistR = cvCreateHist( 1, sizes, CV_HIST_ARRAY, ranges, 1),
			   *IhistB = cvCreateHist( 1, sizes, CV_HIST_ARRAY, ranges, 1),
			   *IhistG = cvCreateHist( 1, sizes, CV_HIST_ARRAY, ranges, 1);
		
	CvCapture* capture;  							
	capture = cvCaptureFromCAM(0);						
	
	image = cvRetrieveFrame( capture,1 );
	imageR=cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
	imageB=cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
	imageG=cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
	
	result=cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 3);
	
	sample=cvLoadImage("p-3_tamplet.jpg",-1);
	sampleR=cvCreateImage(cvSize(sample->width, sample->height), IPL_DEPTH_8U, 1);
	sampleB=cvCreateImage(cvSize(sample->width, sample->height), IPL_DEPTH_8U, 1);
	sampleG=cvCreateImage(cvSize(sample->width, sample->height), IPL_DEPTH_8U, 1);
	
	cvSplit(sample,sampleB,sampleR,sampleG,NULL);
	
	cvCalcHist(&sampleR,ShistR,0,0);
	cvCalcHist(&sampleG,ShistB,0,0);
	cvCalcHist(&sampleB,ShistG,0,0);

	cvNamedWindow( "camera",0);
	cvNamedWindow( "result",0);
	cvCreateTrackbar("loop","result",&loop,32,NULL);		
	
	while(1)
	{
		ucvViewCam(image,capture,"camera");
		
		cvSplit(image,imageB,imageR,imageG,NULL);
		
		cvCalcHist(&imageR,IhistR,0,0);
		cvCalcHist(&imageG,IhistB,0,0);
		cvCalcHist(&imageB,IhistG,0,0);
		
		for(y = 0; y < (image->height); y+=3) 
		{			
			for(x = 0; x < (image->widthStep); x += 3) 
			{
				MR=0;
				MB=0;
				MG=0;
				index = y * image->widthStep + x*3;

				B = image->imageData[index+2];	
				G = image->imageData[index+1];	
				R = image->imageData[index+0];	
				
				for(z=0;z<loop;z++)
				{	
					if(R+z>=256&&R+z<=0&&B+z>=256&&B+z<=0&&G+z>=256&&G+z<=0)break;
					
					MR+= cvGetReal1D( ShistR->bins,R+z )/cvGetReal1D( IhistR->bins,R);
					MB+= cvGetReal1D( ShistB->bins,B+z )/cvGetReal1D( IhistR->bins,B);
					MG+= cvGetReal1D( ShistG->bins,G+z )/cvGetReal1D( IhistG->bins,G);
					
/*				M = (cvGetReal1D( ShistR->bins,R )+cvGetReal1D( ShistB->bins,B )+cvGetReal1D( ShistG->bins,G ))/
					(cvGetReal1D( IhistR->bins,R)+cvGetReal1D( IhistR->bins,B)+cvGetReal1D( IhistG->bins,G));
				
				D=M/3*256;*/
				}
								
				D=(char)((MR+MG+MB)/3*256);
				printf("%d\n",index);
				if(index+2>((image->widthStep)*(image->height)))break;
				result->imageData[index+2] = D;	
				result->imageData[index+1] = D;
				result->imageData[index+0] = D;
			}
		}
		
		cvShowImage( "result",result );
		
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


