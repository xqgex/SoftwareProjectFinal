#include "SPImageProc.h"
extern "C" {
#include "Extraction.h"
#include "main_aux.h"
#include "SPConfig.h"
#include "SPLogger.h"
#include "SPPoint.h"
#include "defines.h"
}
#include <cstdio>
#include <cstdlib>
#include <cstring>


//TODO convert all the info messages literals to #define MSG format
using namespace sp;

int main (int argc, char *argv[]) {
	//tmpFunc1(); //TODO it cause the program to crash, probably memory issue
	char filename[STRING_LENGTH], imagePath[STRING_LENGTH], query[STRING_LENGTH] = {'\0'};
	SP_CONFIG_MSG config_msg = SP_CONFIG_SUCCESS;
	SPConfig config;
	ImageProc *imageProc = NULL;
	int index, numOfImages, numOfFeats;
	SPPoint* featuresArray;
	getFileName(filename,argc,argv);
	config = spConfigCreate(filename, &config_msg); // Load the configuration file
	if (config_msg != SP_CONFIG_SUCCESS) {
		return EXIT_FAILURE;
	}
	if(!initLogger(config)){
		//TODO free memory (config)
		return EXIT_FAILURE;
	}
	imageProc = new ImageProc(config);
	spLoggerPrintInfo(IMAGE_PROC_SUCCESS);
	//TODO move this section into a function in the main.cpp (can't move to main_aux)
	if (spConfigIsExtractionMode(config,&config_msg)) {
		spLoggerPrintInfo(EXTRACTION_MODE_START);
		numOfImages = spConfigGetNumOfImages(config,&config_msg);
		for (index=0;index<numOfImages;index++) {
			config_msg = spConfigGetImagePath(imagePath,config,index);
			if (config_msg != SP_CONFIG_SUCCESS) {
				spLoggerPrintError(GET_IMAGE_PATH_FAIL_ERROR,__FILE__,__func__,__LINE__);
				//TODO free memory (logger + imageProc + config)
				return EXIT_FAILURE;
			}
			featuresArray = imageProc->getImageFeatures(imagePath,index,&numOfFeats);
			if (featuresArray == NULL) {
				spLoggerPrintError(FEATURES_EXTRACTION_FROM_IMAGE_FAIL_ERROR,__FILE__,__func__,__LINE__);
				//TODO free memory (logger + imageProc + config)
				return EXIT_FAILURE;
			}
			config_msg = spConfigGetFeatsPath(imagePath,config,index);
			if (config_msg != SP_CONFIG_SUCCESS) {
				spLoggerPrintError(GET_FEATS_PATH_FAIL_ERROR,__FILE__,__func__,__LINE__);
				//TODO free memory (logger + imageProc + config)
				return EXIT_FAILURE;
			}
			if (!ExportFeats(imagePath,featuresArray,numOfFeats)) {
				//TODO free memory (logger + imageProc + config + featuresArray and his elements)
				return EXIT_FAILURE;
			}
		}
		spLoggerPrintInfo(EXTRACTION_MODE_SUCCESS);
	}
	// TODO import the extracted features
	// TODO build the KDtree
	while (strcmp(query,"<>") != 0) {
		printf(QUERY_IMG_MSG);
		fflush(stdout);
		scanf("%1024s",query);
		//TODO check if the query path is valid
		//TODO get features of the new image
		//TODO find matches (KDTree search)
		//TODO show matches (GUI or stdout)
		//TODO reset variable of the current query (clear the features)
	}
	printf(EXIT_MSG);
	fflush(stdout);
	//TODO free memory
	delete imageProc;
	spConfigDestroy(config);
	//TODO figure out why the program crash here instead of peacefully end
	return (EXIT_SUCCESS);
}
