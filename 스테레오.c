#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include<stdbool.h>

#define CenterX 310
#define CenterY 250
#define FoucusLenth 710
#define CameraDistance 2840
#define Pixel 10
#define L 8

CvRect box;	
CvRect sbox;																		//구조체 참조 선언 
bool DrawingBox = false;														//이벤트 논리변수 
bool RoiSet = false;

void ucvViewCam( IplImage*Image, CvCapture* capture, char *WindowName );
void ucvViewImage( char *WindowName, IplImage *image );

void MouseCallBack( int event, int x, int y, int flags, void*param );				//마우스이벤트정의함수 선 정의 
void DrawBox( IplImage*img, CvRect rect );											//사각형그리기 박스 선 정의 

int main()
{
	box = cvRect(-1,-1,0,0);
	
	unsigned char R,B,G,Weight1,Weight2;	
	
	int x,y,Index,
		BoxCenterX1=150,BoxCenterY1=150,
		BoxCenterX2=150,BoxCenterY2=150,
		PointX1,PointX2,PointY1,PointY2,
		Size=65,i,j,
		SigmaX1=0,SigmaY1=0,
		SigmaX2=0,SigmaY2=0,
		SigmaW1,SigmaW2,
		Lenth=0,X=0,Y=0,Z=0,K=0;

	double P=0,
		   Rhist[L],
		   Bhist[L],
		   Ghist[L];
	
	int bins = 256;
	int Sizes[] = {bins};
	float xranges[] = {0, 256};
	float*ranges[] = {xranges};
	
	IplImage*Input1,*Input2,
			*ModelR,*ModelG,*ModelB,*Model,
			*Output1,*Output2,
			*DrawingCopy,*OutputCopy1,*OutputCopy2;
		
	CvHistogram*ShistR = cvCreateHist( 1, Sizes, CV_HIST_SPARSE, ranges, 1),
			   *ShistB = cvCreateHist( 1, Sizes, CV_HIST_SPARSE, ranges, 1),
			   *ShistG = cvCreateHist( 1, Sizes, CV_HIST_SPARSE, ranges, 1);
			   
	CvCapture * Capture1 = cvCaptureFromCAM(0),
			  * Capture2 = cvCaptureFromCAM(1);						
	
	Input1 = cvRetrieveFrame( Capture1,1 );
	Input2 = cvRetrieveFrame( Capture2,1 );
	DrawingCopy = cvCloneImage( Input1 );
	OutputCopy1 = cvCloneImage( Input1 );
	OutputCopy2 = cvCloneImage( Input2 );
	Output1=cvCreateImage( cvSize( Input1->width, Input1->height ), IPL_DEPTH_8U, 3 );
	Output2=cvCreateImage( cvSize( Input2->width, Input2->height ), IPL_DEPTH_8U, 3 );

	cvNamedWindow( "camera", 0 );



	if( Capture1 )										//캡쳐값이 존재할경우
	{	
    	cvNamedWindow( "camera", 1 );					//윈도우 생성

        cvSetMouseCallback( "camera",MouseCallBack,(void*)Input1 );	
        
		while(1)										
        {
            if( !cvGrabFrame( Capture1 ) )				//프레임이 드롭되면 프로그램 종료 
                break;
			
			if(DrawingBox==false)
				Input1 = cvRetrieveFrame( Capture1,1 );		//Input1에 capture에 받아온 단일 프레임을 저장 		
			
			cvCopy(Input1,DrawingCopy,0);
			
            if(DrawingBox)
			{	
				cvCopy( Input1,DrawingCopy,0 );										//복사이미지에 드로잉 
				DrawBox( DrawingCopy,box );
			}
					
			if(RoiSet == true && DrawingBox == false)
				break;
					
			cvShowImage( "camera", DrawingCopy );				//Input1를 camera창에 표시  
			
			if( cvWaitKey(15)==27 )break;
        }
        cvSetImageROI( DrawingCopy, box );

		BoxCenterX1=box.x+Size;
		BoxCenterY1=box.y+Size;
		
		cvSaveImage( "Model.jpg", DrawingCopy,0 );
		Model=cvLoadImage( "Model.jpg", -1 );
		
		ModelR=cvCreateImage( cvSize( Model->width, Model->height ), IPL_DEPTH_8U, 1 );
		ModelB=cvCreateImage( cvSize( Model->width, Model->height ), IPL_DEPTH_8U, 1 );
		ModelG=cvCreateImage( cvSize( Model->width, Model->height ), IPL_DEPTH_8U, 1 );
		
		cvSplit( Model, ModelB, ModelR, ModelG, NULL );
		
		cvCalcHist( &ModelR, ShistR, 0, 0 );
		cvCalcHist( &ModelG, ShistG, 0, 0 );
		cvCalcHist( &ModelB, ShistB, 0, 0 );
		
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

		while(1)
		{
			
			SigmaX1=0;
			SigmaY1=0;
			SigmaW1=0;
			SigmaX2=0;
			SigmaY2=0;
			SigmaW2=0;
			Weight1=0;
			Weight2=0;
			
			Input1 = cvRetrieveFrame( Capture1,0 );
			Input2 = cvRetrieveFrame( Capture2,0 );
							
			for( y = 0 ; y < ( Input1->height ) ; y += 4 ) 
			{			
				for( x = 0 ; x < ( Input1->widthStep ) ; x += 4 ) 
				{
					Index = y * Input1->widthStep + x * 3;
					
					R = Input1 -> imageData[Index+2];	
					G = Input1 -> imageData[Index+1];	
					B = Input1 -> imageData[Index+0];	
		
					
					Weight1 = (unsigned char)(pow((Rhist[R/(256/L)]*Bhist[B/(256/L)]*Ghist[G/(256/L)]), 1.0/3.0)/
											  (Rhist[R/(256/L)]+Bhist[B/(256/L)]+Ghist[G/(256/L)])*3*255);
						
					if( BoxCenterX1 - Size < x && x < BoxCenterX1 + Size && BoxCenterY1 - Size < y && y < BoxCenterY1 + Size )
					{
						SigmaX1 = SigmaX1 + Weight1 * x ;						
						SigmaY1 = SigmaY1 + Weight1 * y ;
							
						SigmaW1 += Weight1;      									
					}						
						
					Output1 -> imageData[Index+2] = Weight1;	
					Output1 -> imageData[Index+1] = Weight1;
					Output1 -> imageData[Index+0] = Weight1;
					
				}	
			}
			
			for( y = 0 ; y < ( Input2 -> height ) ; y += 4 ) 
			{			
				for( x = 0 ; x < (Input2->widthStep) ; x += 4 ) 
				{
					Index = y * Input2->widthStep + x*3;
					
					R = Input2 -> imageData[Index+2];	
					G = Input2 -> imageData[Index+1];	
					B = Input2 -> imageData[Index+0];	
		
					
					Weight2 = (unsigned char)(pow((Rhist[R/(256/L)]*Bhist[B/(256/L)]*Ghist[G/(256/L)]), 1.0/3.0)/
										     (Rhist[R/(256/L)]+Bhist[B/(256/L)]+Ghist[G/(256/L)])*3*255);
					
					if( BoxCenterX2 - Size < x && x < BoxCenterX2 + Size && BoxCenterY2 - Size < y && y < BoxCenterY2 + Size )
					{
						SigmaX2 = SigmaX2 + Weight2 * x;						
						SigmaY2 = SigmaY2 + Weight2 * y;
							
						SigmaW2 += Weight2;      									
					}		
										
					Output2 -> imageData[Index+2] = Weight2;	
					Output2 -> imageData[Index+1] = Weight2;
					Output2 -> imageData[Index+0] = Weight2;
					
				}	
			}
				
			if( SigmaW1 > 0 )
			{	
				BoxCenterX1 = ( SigmaX1 / SigmaW1 );
				BoxCenterY1 = ( SigmaY1 / SigmaW1 );
				
				PointX1 = BoxCenterX1 - CenterX;
				PointY1 = BoxCenterY1 - CenterY;
			}	
			
			if( SigmaW2 > 0 )
			{	
				BoxCenterX2 = ( SigmaX2 / SigmaW2 );
				BoxCenterY2 = ( SigmaY2 / SigmaW2 );
				
				PointX2 = BoxCenterX2 - CenterX;
				PointX2 = BoxCenterY2 - CenterY;
			}	
			
			K = CameraDistance/sqrt((PointX1-PointX2)^2+(PointY1-PointY2)^2);
			
			X=(PointX1+PointX2)*K;
			Y=(PointY1+PointY2)*K;
			Z=FoucusLenth*2*K;
			
			Lenth=sqrt(X^2+Y^2+Z^2);
			
			printf("cam1; %d,%d // cam2; %d,%d // lenth %d\n",PointX1,PointY1,PointX2,PointX2,Lenth);
			
			cvCopy( Output1, OutputCopy1, 0 );	
			cvCopy( Output2, OutputCopy2, 0 );	
			
			cvRectangle( OutputCopy1, cvPoint( BoxCenterX1 - Size, BoxCenterY1 - Size ), 
						 cvPoint( BoxCenterX1 + Size, BoxCenterY1 + Size ), CV_RGB( 255,0,0 ), 5, 0, 0 );
						 
			cvRectangle( Input1, cvPoint( BoxCenterX1 - Size, BoxCenterY1 - Size ), 
						 cvPoint( BoxCenterX1 + Size, BoxCenterY1 + Size ), CV_RGB( 255,0,0 ), 5, 0, 0 );
			
			cvRectangle( OutputCopy2, cvPoint( BoxCenterX2 - Size, BoxCenterY2 - Size ), 
						 cvPoint( BoxCenterX2 + Size, BoxCenterY2 + Size ), CV_RGB( 255,0,0 ), 5, 0, 0 );
						 
			cvRectangle( Input2, cvPoint( BoxCenterX2 - Size, BoxCenterY2 - Size), 
						 cvPoint( BoxCenterX2 + Size, BoxCenterY2 + Size ), CV_RGB( 255,0,0 ), 5, 0, 0 );
			
			ucvViewImage( "result1", OutputCopy1 );
			ucvViewImage( "result2", OutputCopy2 );

			ucvViewImage( "camera1", Input1 );
			ucvViewImage( "camera2", Input2 );
			
			if( cvWaitKey(10) >= 0 )
		        break;
		}	

        cvReleaseCapture( &Capture1 );					//이미지 릴리스 
        cvReleaseCapture( &Capture2 );	
		cvReleaseImage( &Input1 );
		cvReleaseImage( &Input2 );
		cvReleaseImage( &DrawingCopy );

        cvDestroyAllWindows();
	}
}

void MouseCallBack(int event, int x, int y, int flags, void*param)
{
	IplImage*Image=(IplImage*)param;
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

void ucvViewImage(char *WindowName, IplImage *image)
{
	cvNamedWindow( WindowName, 0);
	cvShowImage( WindowName, image );
}
