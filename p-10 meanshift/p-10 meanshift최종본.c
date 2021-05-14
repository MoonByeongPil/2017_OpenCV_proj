#include <cv.h>
#include <highgui.h>
#include <stdio.h>

int main() 
{
	IplImage *image,														// 원본영상
			 *color,															// 색 검출 영상
			 *temp;
			 	
	int y, x, index,
		BR=21,BG=21,Bc=170,	
		centerX=150,centerY=150,
		size=65,sigmaX=0,sigmaY=0,
		numberX=0,numberY=0;
		
	unsigned char R, G, B;
	
	CvCapture* capture = cvCaptureFromCAM(0);
	
	image = cvRetrieveFrame(capture,0);
	color = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
	temp = cvCloneImage(color);
	
	cvNamedWindow("Original",1);
	cvNamedWindow("Tracking",1);
	cvCreateTrackbar("B-R","Tracking",&BR,255,NULL);
	cvCreateTrackbar("B-G","Tracking",&BG,255,NULL);
	cvCreateTrackbar("Bc","Tracking",&Bc,255,NULL);
	cvCreateTrackbar("size","Tracking",&size,100,NULL);					
	FILE *pFile = NULL;                                      ///  파일 생성 pfile의 주소를 포인터 변수로 선언	
    pFile = fopen( "move.txt", "w+" );					////  파일 내용과 형식 지정,  W라는 형식의 스트리밍을 사용하겠다라고 move.txt 지정 
    if( pFile != NULL )									////   파일이 생성되지 않았을 때 실행하지 말것 
    {
        
		while(capture && (!feof( pFile ))) 					/// 파일이 열리지 않을 때 실행하지 말 것 capture&는 카메라가 열리지 않아도 실행되지 않음    
		{
			sigmaX=0;
			sigmaY=0;
			numberX=0;
			numberY=0;
			
			image = cvRetrieveFrame(capture,0);
			
			for(y = 0; y < (image->height); y+=3) 
			{			
				for(x = 0; x < (image->widthStep); x += 3) 
				{
					index = y * image->widthStep + x*3;
					
					R = image->imageData[index+2];	
					G = image->imageData[index+1];	
					B = image->imageData[index+0];	
					
					if(image)
					{
						if(B-R > BR && B-G > BG && B > Bc) 
						{
							color->imageData[index+0] = 255;	
							color->imageData[index+1] = 255;
							color->imageData[index+2] = 255;
							
							if(centerX-size<x&&x<centerX+size&&centerY-size<y&&y<centerY+size)
							{
								if((centerX-x)^2<size)	sigmaX=sigmaX+x;						
								if((centerX-y)^2<size)	sigmaY=sigmaY+y;
									
								numberX=numberX+1;      
								numberY=numberY+1;							
							}									
						}
						else 
						{
							color->imageData[index+0] = 0;
							color->imageData[index+1] = 0;
							color->imageData[index+2] = 0;
						}
					}
				}
			}		
			
			if(numberX>0&&numberY>0)
			{	
				centerX=sigmaX/numberX;
				centerY=sigmaY/numberY;
			}	
			
			fputs( "0", pFile );											/// pfile  에 0을 써라, 포인터는 컴퓨터 내의 pfile위치 기억시킴 
			fflush(pFile);													/// 쓰고 나서 데이터 남으면 오류남기에 데이터 삭제(스트림에 남아있는 파일 삭제), txt지우는 것 아님  
			
			cvCopy(color,temp,0);	
			cvRectangle(temp, cvPoint(centerX-size,centerY-size),cvPoint(centerX+size, centerY+size), CV_RGB(255,0,0),5,0,0);
			cvRectangle(image, cvPoint(centerX-size,centerY-size),cvPoint(centerX+size, centerY+size), CV_RGB(255,0,0),5,0,0);
	
			
			cvShowImage("Original",image);
			cvShowImage("Tracking",temp);
			
			if(cvWaitKey(33) >= 27)	break;
		}
		fclose( pFile );   													/// 파일 닫기 
	}
	cvReleaseCapture(&capture);
	cvReleaseImage(&image);
	cvReleaseImage(&temp);
	cvReleaseImage(&color);
	cvDestroyAllWindows();

	return 0;
}
