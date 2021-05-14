#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define L 16
CvRect box;	
CvRect sbox;
																		//구조체 참조 선언 
bool DrawingBox = false;														//이벤트 논리변수 
bool RoiSet=false;

void ucvViewCam(IplImage*Image, CvCapture* capture, char *WindowName);
void ucvViewImage(char *WindowName, IplImage *Input1);
void histogram(char* w_name, IplImage* img);

void MouseCallBack(int event, int x, int y, int flags, void*param);				//마우스이벤트정의함수 선 정의 
void DrawBox(IplImage*img,CvRect rect);											//사각형그리기 박스 선 정의 

int main()
{
	box = cvRect(-1,-1,0,0);

	unsigned char R,B,G,Weight=0;	
	
	int x,y,Index,
		BoxCenterX=150,BoxCenterY=150,Size=65,
		i,j,
		SigmaX=0,SigmaY=0,SigmaW=0;	

	int bins = 256;
	int Sizes[] = {bins};
	float xranges[] = {0, 256};
	float*ranges[] = {xranges};
	
	double P=0,
		   Rhist[L],
		   Bhist[L],
		   Ghist[L];
	
	IplImage*Input1,*Output1,*OutputCopy1,
			*ModelR,*ModelG,*ModelB,*Model,
			*DrawingCopy;
		
	CvHistogram*ShistR = cvCreateHist( 1, Sizes, CV_HIST_SPARSE, ranges, 1),
			   *ShistB = cvCreateHist( 1, Sizes, CV_HIST_SPARSE, ranges, 1),
			   *ShistG = cvCreateHist( 1, Sizes, CV_HIST_SPARSE, ranges, 1);
			   
	CvCapture* Capture1 = cvCaptureFromCAM(0);						
	
	Input1 = cvRetrieveFrame( Capture1,1 );
	DrawingCopy = cvCloneImage(Input1);
	OutputCopy1 = cvCloneImage(Input1);
	Output1=cvCreateImage(cvSize(Input1->width, Input1->height), IPL_DEPTH_8U, 3);

	cvNamedWindow( "camera",0);

	for(i=0;i<L;i++)
	{
		Rhist[i]=0;
		Bhist[i]=0;
		Ghist[i]=0;
	}
	
	if( Capture1 )										//캡쳐값이 존재할경우
	{	
    	cvNamedWindow( "camera", 1 );					//윈도우 생성

        cvSetMouseCallback("camera",MouseCallBack,(void*)Input1);	
        
		while(1)										
        {
            if( !cvGrabFrame( Capture1 ))				//프레임이 드롭되면 프로그램 종료 
                break;
			
			if(DrawingBox==false)
				Input1 = cvRetrieveFrame( Capture1,1 );		//Input1에 capture에 받아온 단일 프레임을 저장 		
			
			cvCopy(Input1,DrawingCopy,0);
			
            if(DrawingBox)
			{	
				cvCopy(Input1,DrawingCopy,0);										//복사이미지에 드로잉 
				DrawBox(DrawingCopy,box);
			}
					
			if(RoiSet==true&&DrawingBox==false)
				break;
					
			cvShowImage( "camera", DrawingCopy );				//Input1를 camera창에 표시  
			
			if(cvWaitKey(15)==27)
			{
				return 0;
			}
        }
        cvSetImageROI(DrawingCopy, box);


		BoxCenterX=box.x+Size;
		BoxCenterY=box.y+Size;
		
		cvSaveImage( "Model.jpg",DrawingCopy);
		Model=cvLoadImage( "Model.jpg",-1);
		ModelR=cvCreateImage(cvSize(Model->width, Model->height), IPL_DEPTH_8U, 1);
		ModelB=cvCreateImage(cvSize(Model->width, Model->height), IPL_DEPTH_8U, 1);
		ModelG=cvCreateImage(cvSize(Model->width, Model->height), IPL_DEPTH_8U, 1);
		
		cvSplit(Model,ModelB,ModelG,ModelR,NULL);
		cvNamedWindow( "Model",0);
		cvShowImage( "Model", ModelG );
		
		cvCalcHist(&ModelR,ShistR,0,0);
		cvCalcHist(&ModelG,ShistG,0,0);
		cvCalcHist(&ModelB,ShistB,0,0);
		
		histogram("R",ModelR);
		histogram("B",ModelB);
		histogram("G",ModelG);
			
		P=Model->width*Model->height;
		for(i=0;i<L;i++)
		{
			for(j=0;j<256/L;j++)
			{
				Rhist[i]+=(cvGetReal1D( ShistR->bins,j+256/L*i )/P);
				Bhist[i]+=(cvGetReal1D( ShistB->bins,j+256/L*i )/P);
				Ghist[i]+=(cvGetReal1D( ShistG->bins,j+256/L*i )/P);
			}			
			printf("%f,%f,%f",Rhist[i],Bhist[i],Ghist[i]);
			printf("\n");
		}
		printf("\n");

		double r=0,b=0,g=0;
		int ra=0,ba=0,ga=0,w;
		for(j=0;j<L;j++)
		{
			if(Rhist[j]>r){ r=Rhist[j]; ra=j;printf("a");}
			if(Bhist[j]>b){ b=Bhist[j]; ba=j;printf("a");}
			if(Ghist[j]>g){ g=Ghist[j]; ga=j;printf("a");}
		printf("\n");
		printf("%d %f,%f,%f  %d,%d,%d",j,Rhist[j],Bhist[j],Ghist[j],ra,ba,ga);
		}			
		
		printf("\n");
		printf("%d,%d,%d",ra,ba,ga);

		
		while(1)
		{			
			SigmaX=0;
			SigmaY=0;
			SigmaW=0;
			Weight=0;
			
			Input1 = cvRetrieveFrame(Capture1,0);
			
			
			for(y = 0; y < (Input1->height); y+=3) 
			{			
				for(x = 0; x < (Input1->widthStep); x += 3) 
				{
					Index = y * Input1->widthStep + x*3;
					
					R = Input1->imageData[Index+2];	
					G = Input1->imageData[Index+1];	
					B = Input1->imageData[Index+0];	
					
					w =(pow((R/L-ra),2)+pow((B/L-ba),2)+pow((G/L-ga),2))/60;
					if(w<0.7) Weight=128-128*w;
					else Weight=0;
					//(pow((Rhist[R/(256/L)]*Bhist[B/(256/L)]*Ghist[G/(256/L)]), 1.0/3.0)/
							//			  (Rhist[R/(256/L)]+Bhist[B/(256/L)]+Ghist[G/(256/L)])*3*255);
					
					
					if(BoxCenterX-Size<x&&x<BoxCenterX+Size&&BoxCenterY-Size<y&&y<BoxCenterY+Size)
					{
						SigmaX=SigmaX+Weight*x;						
						SigmaY=SigmaY+Weight*y;
							
						SigmaW+=Weight;      		
					}						
						
					Output1->imageData[Index+2] = Weight;	
					Output1->imageData[Index+1] = Weight;
					Output1->imageData[Index+0] = Weight;				
				}	
			}
				
			if(SigmaW>0)
			{	
				BoxCenterX=SigmaX/SigmaW;
				BoxCenterY=SigmaY/SigmaW;
			}	


			
			cvCopy(Output1,OutputCopy1,0);	
			cvRectangle( OutputCopy1, cvPoint(BoxCenterX-Size,BoxCenterY-Size),cvPoint(BoxCenterX+Size, BoxCenterY+Size), CV_RGB(255,0,0),5,0,0);
			cvRectangle( Input1, cvPoint(BoxCenterX-Size,BoxCenterY-Size),cvPoint(BoxCenterX+Size, BoxCenterY+Size), CV_RGB(255,0,0),5,0,0);
			
			ucvViewImage("result",OutputCopy1);
			ucvViewImage("camera",Input1);
			
			if( cvWaitKey(10) >= 0 )
		        break;
		}	


        cvReleaseCapture( &Capture1 );					//이미지 릴리스 
		cvReleaseImage( &Input1 );
		cvReleaseImage( &DrawingCopy );					
        cvDestroyAllWindows();
	}
}

void MouseCallBack(int event, int x, int y, int flags, void*param)
{
	IplImage*Input1=(IplImage*)param;
	switch(event)
	{
		case CV_EVENT_MOUSEMOVE:												//마우스가 움직이면 박스의 길이는 초기x-마우스x 
			{
				if(DrawingBox)
				{
					box.width = x - box.x;
					box.height = y - box.y;
				}
			}break;
			
		case CV_EVENT_LBUTTONDOWN:												//마우스눌리면 드로잉 
			{
				DrawingBox=true;
				box = cvRect(x,y,0,0);
			}break;
			
		case CV_EVENT_LBUTTONUP:												//마우스때면 드로잉 종료 
			{
				DrawingBox=false;
				if(box.width<0)													//반대방향일경우 절댓값 
				{
					box.x+=box.width;
					box.width*=-1;
				}
				
				if(box.height<0)
				{
					box.y+=box.height;
					box.height*=-1;
				}
				RoiSet=true;
				
			}break;
	}
}

void DrawBox(IplImage*img,CvRect rect)
{
	cvRectangle(img,cvPoint(rect.x,rect.y),cvPoint(rect.x+rect.width,rect.y+rect.height),CV_RGB(255,0,0),5,0,0);
}

void ucvViewCam(IplImage*Image, CvCapture* capture, char *WindowName)
{
	if( capture )										//캡쳐값이 존재할경우 
	{    					
        Image = cvRetrieveFrame( capture,1 );		//Image에 capture에 받아온 단일 프레임을 저장 
		cvShowImage( WindowName, Image );				//Image를 camera창에 표시  		     
    }
}

void ucvViewImage(char *WindowName, IplImage *Image)
{
	cvNamedWindow( WindowName, 0);
	cvShowImage( WindowName, Image );
}


void histogram(char* w_name, IplImage* img)
{
	IplImage* canvas;
	int       hist[256];
	double    scale = 1;
	int       i, j, max = 0;

	canvas = cvCreateImage(cvSize(256, 256), IPL_DEPTH_8U, 1);
	cvSet(canvas, CV_RGB(255, 255, 255), 0);

	for (i = 0; i < 256; i++)
	{
		hist[i] = 0;
	}
	
	for (i = 0; i < img->height; i++)
	{
		for (j = 0; j < img->width; j++)
		{
			hist[(uchar)img->imageData[(i * img->widthStep) + j]]++;
		}
	}	
	max = 256 * 50;		
	scale = max > canvas->height ? (double)canvas->height/max : 1.;

	for (i = 0; i < 256; i++)
	{
		CvPoint pt1 = cvPoint(i, canvas->height - (hist[i] * scale));
		CvPoint pt2 = cvPoint(i, canvas->height);
		cvLine(canvas, pt1, pt2, CV_RGB(0,0,0), 1, 8, 0);
	}

	cvShowImage(w_name, canvas);

	cvReleaseImage(&canvas);
}
