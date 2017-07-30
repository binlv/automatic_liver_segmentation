/*********************************************
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Automatic Liver Segmentation Project
Part I: Automatic Liver Detection
Torecan CELIK
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**********************************************/

#include "auto_header.h"


IplImage *im;

void saveImage(IplImage*, int );

class autodetect{

	int width, height, step, channel;				// Properties of image
	int label, max_lab;							    // For labelling func.
	uchar* data;									// Data of image;


public: 

			autodetect(IplImage* img,int num){
			width = img->width;
			height = img->height;
			step = img->widthStep;
			channel = img->nChannels;
			data = (uchar*)img->imageData;
			

			cvShowImage("ORIGINAL IMAGE", img);
			IplImage *orginalImage = cvCloneImage(img);		// Original image will multiples with liver detected image 
			
			morphologic(img);								// Some morphologic processes such as erosion, thresholding
			cvShowImage("THRESHOLDED IMAGE", img);
			
			labelling(img);									// Labelling shows us likely liver candidate places
			cvShowImage("LABELLED IMAGE", img);

			maskLiver(img);							// Find the biggest labelled part of image (among probable liver candidate)
			cvShowImage("mas", img);
			cvMul(img, orginalImage, orginalImage);			// Original image multiples with binary labelled image
			cvShowImage("DETECTED IMAGE", orginalImage);
			saveImage(orginalImage, num);

}
		
		void morphologic(IplImage*);
		void fillHoles(uchar*);
		void labelling(IplImage*);
		void histogramLabelled(IplImage*, int);
		void maskLiver(IplImage*);


};

void saveImage(IplImage *image, int number)
{
	// Store path to directory
	char filename[100];
	strcpy(filename, "C://Users//TOR//Desktop//Ne//resim.");

	// Convert integer to char    
	char frameNo[10];
	sprintf(frameNo, "%02i", number);

	// Combine to generate path
	strcat(filename, frameNo);
	strcat(filename, ".jpg");

	// Use path to load image

	cvSaveImage(filename, image);

	//cvShowImage(frameNo, image);

}

void loadImage(IplImage *image, int number)
{
	// Store path to directory
	char filename[100];
	strcpy(filename, "C://Users//TOR//Desktop//TEZ//Thesis//data//liver_all//CT Abdomen.");

	// Convert integer to char    
	char frameNo[10];
	sprintf(frameNo, "%03i", number);

	// Combine to generate path
	strcat(filename, frameNo);
	strcat(filename, ".jpg");

	// Use path to load image
	image = cvLoadImage(filename,0);

	

	im = cvCloneImage(image);

	//cvShowImage(frameNo, image);

}




int main() {

	int i;
	clock_t x = clock();
	char file[100];

	for ( i= 101; i < 288; i++){
	
		loadImage(im, i);
		autodetect autodetect(im,i);												// go to autodetect class
 	}

	cout << i << "images are processed!.. "	<<"Process time: "<< (clock() - x) / (double)CLOCKS_PER_SEC;		// calculate process time
	
	cvWaitKey();
	cvReleaseImage(&im);
}




// Morphologic Processes
void autodetect::morphologic(IplImage* img){
	cvThreshold(img, img, 117, 255, CV_THRESH_TOZERO);							 // Liver generally consists of an internal that between 117-132 in gray scale 
	cvThreshold(img, img, 132, 255, CV_THRESH_TOZERO_INV);
	cvThreshold(img, img, 117, 255, CV_THRESH_BINARY);							 // Image transforms a binary image	
	cvErode(img, img, 0, 1);													 // 0 represents the 3x3 clustering, 1 is iteration
	cvFloodFill(img, cvPoint(0, 0), cvScalar(150), cvScalar(5), cvScalar(5));	 // FloodFill() func. helps to find the holes
	fillHoles(data);															 // Fills the holes
	cvThreshold(img, img, 200, 255, CV_THRESH_BINARY);
	cvSmooth(img, img, CV_MEDIAN, 7);
}


// After Morphologic Processes there are some holes on the liver, then we fillHoles() function will fill the holes.  
void autodetect::fillHoles(uchar* data){
	for (int i = 0; i < height; i++)
	for (int j = 0; j < width; j++)
	if (data[i*step + j] == 0){ data[i*step + j] = 255; }
}


// labelling func. used to label the connected components
void autodetect::labelling(IplImage*img){
	label = 1;

	for (int i = 2; i < height; i++)
	for (int j = 2; j < width; j++){
		if (data[i*step + j] == 255){
			cvFloodFill(img, cvPoint(j, i), cvScalar(label), cvScalar(1), cvScalar(1));	 // FloodFill() func. helps to find the holes
			label++;
		}
	}
	histogramLabelled(img, label);
}



// histogramLabelled funct. used to find out the label of candidate liver region
void autodetect::histogramLabelled(IplImage* img, int label){

	int *hist;
	hist = new int[label + 1];
	int max = 0;
	
	for (int i = 0; i < label + 1; i++)				// Assign first values (0) of histogram 
		hist[i] = 0;

	for (int la = 1; la < label + 1; la++)
	for (int i = 1; i < height; i++)
	for (int j = 1; j < width; j++){
		if (data[i*step + j] == la)
			hist[la]++;								// Histogram of labelled image
	}

	for (int i = 1; i < label + 1; i++){
		if (hist[i]>max){
			max = hist[i];
			max_lab = i;							// Find out the label of biggest part of labelled image
		}
	}
	

}


// maskLiver funct. used to mask probable liver part binary 1, else pixels turn binary 0 
void autodetect::maskLiver(IplImage*img){

	for (int i = 1; i < height; i++)
	for (int j = 1; j < width; j++){

		if (data[i*step + j] == max_lab){
			data[i*step + j] = 1;}
		else{ data[i*step + j] = 0;}

	}

}
