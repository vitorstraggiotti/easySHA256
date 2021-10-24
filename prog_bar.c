/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	Library to render progress bar on terminal
	
	Author: Vitor Henrique Andrade Helfensteller Straggiotti Silva
	Date: 18/09/2021  (DD/MM/YYYY)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "prog_bar.h"

//ANSI scape codes
#define ESC  "\033"			/* Escape to listen intruction */
#define CSI  "["			/* Use "Control Sequence Introducer" */

#define RED_BG  "101m"		/* Bright red character background */
#define BLUE_BG "104m"		/* Bright blue character background */
#define RESET  "0m"			/* Reset foregroung and background colors */

#define LINEUP2 "2F"		/* Moves cursor to beginning of the second line up */
#define LINEUP1 "1F"		/* Moves cursor to beginning of the first line up */

/* Hold information for the correct dimensional representation */
struct bar
{
	int64_t CurrState;
	int64_t TargetState;	/* Next target value for "CurrState" to update bar */
	int64_t Min;
	int64_t Max;
	uint8_t BarSize;
	uint8_t Precision;		/* >= 0 Number of decimal places on percentage */
	uint64_t NumOfSlices;	/* 100.0/Precision */
	uint64_t SliceSize;		/* [(Max-Min)/NumOfSlices] >= 1 */
};
typedef struct bar bar_t;

/* Hold information on how to visually represent the progress bar */
struct bar_graph
{
	char Start;
	char FullBody;
	char EmptyBody;
	char Finish;
};
typedef struct bar_graph bar_graph_t;


/******************************************************************************/
/* Constructor for the progress bar object */
bar_t *init_bar(int64_t Min, int64_t Max, uint8_t BarSize, uint8_t Precision)
{
	bar_t *Bar;
	uint8_t Aux;
	
	Bar = malloc(sizeof(bar_t));
	
	Bar->CurrState = Min;
	Bar->TargetState = Min;
	Bar->Min = Min;
	Bar->Max = Max;
	Bar->BarSize = BarSize;
	Bar->Precision = Precision;
	
	/* Setting Number of slices */
	Aux = Precision;
	Bar->NumOfSlices = 100;
	while(Aux > 0)
	{
		Bar->NumOfSlices *= 10;
		Aux--;
	}
	
	/* Setting slice size */
	if((Max - Min) >= Bar->NumOfSlices)
	{
		Bar->SliceSize = (Max - Min)/Bar->NumOfSlices;
	}
	else
	{
		Bar->NumOfSlices = Max - Min;
		Bar->SliceSize = 1;
	}
	
	return Bar;
}
/******************************************************************************/
/* Constructor for the graphical object for the progress bar */
bar_graph_t *init_bar_graph(char Start, char FullBody, char EmptyBody, char Finish)
{
	bar_graph_t *Graph;
	
	Graph = malloc(sizeof(bar_graph_t));
	
	/* Testing if printable. If not, use default value */
	if((Start >= 0x20) && (Start <= 0x7e))
		Graph->Start = Start;
	else
		Graph->Start = '[';
	
	if((FullBody >= 0x20) && (FullBody <= 0x7e))
		Graph->FullBody = FullBody;
	else
		Graph->FullBody = '=';
	
	if((EmptyBody >= 0x20) && (EmptyBody <= 0x7e))
		Graph->EmptyBody = EmptyBody;
	else
		Graph->EmptyBody = ' ';
	
	if((Finish >= 0x20) && (Finish <= 0x7e))
		Graph->Finish = Finish;
	else
		Graph->Finish = ']';
	
	return Graph;
}
/******************************************************************************/
/* Destructor for the progress bar object */
void destroy_bar(bar_t *Bar)
{
	free(Bar);
}
/******************************************************************************/
/* Destructor for the progress bar graphical object */
void destroy_graph(bar_graph_t *Graph)
{
	free(Graph);
}

/******************************************************************************/
/*                       HELPER FUNCTIONS                                     */
/******************************************************************************/
/* Helper function to draw the progress bar */
static void drawing_bar(bar_graph_t *Graph, uint8_t BarSize,
								uint8_t BarPosition, float Percentage, uint8_t Precision)
{
	fputc('\r', stdout);
	fputc(Graph->Start, stdout);
	for(uint8_t i = 0; i < BarSize; i++)
	{
		if(i < BarPosition)
		{
			fputc(Graph->FullBody, stdout);
		}else
		{
			fputc(Graph->EmptyBody, stdout);
		}
	}
	fputc(Graph->Finish, stdout);
	printf(" %.*f %%", Precision, Percentage);
	fflush(stdout);
}

/******************************************************************************/
/* Helper function to draw more complicated progress bar */
#if 0
static drawing_class_B_bar(char Start, char Body, char Finish, int32_t BodyBGColor,
							int32_t )
{
	
	fputs("\r[", stdout);
	for(uint8_t i = 0; i < BarSize; i++)
	{
		if(i < BarPosition)
		{
			printf(ESC CSI RED_BG " ");
		}else
		{
			printf(ESC CSI BLUE_BG " ");
		}
	}
	printf(ESC CSI RESET "] %.3f %%", Percentage);
	fflush(stdout);
}
#endif
/******************************************************************************/
/*                       GENERIC METHODS                                      */
/******************************************************************************/
/* Method for printing progress bar info */
void show_info_bar(bar_t *Bar)
{
	printf("Current Satate: %ld\n", Bar->CurrState);
	printf("Target State: %ld\n", Bar->TargetState);
	printf("Min: %ld\n", Bar->Min);
	printf("Max: %ld\n", Bar->Max);
	printf("Bar Size: %u\n", Bar->BarSize);
	printf("Precision: %u\n", Bar->Precision);
	printf("Number of Slices: %lu\n", Bar->NumOfSlices);
	printf("Slice Size: %lu\n", Bar->SliceSize);
}

/******************************************************************************/
/* Method for updating the progress bar */
void update_bar(bar_t *Bar, bar_graph_t *Graph, int64_t CurrState)
{
	Bar->CurrState = CurrState;
	if((Bar->CurrState >= Bar->TargetState) || (Bar->CurrState == Bar->Max))
	{
		/* Update next target state to  redraw progress bar */
		if(Bar->CurrState != Bar->Max)
			Bar->TargetState += Bar->SliceSize;
		
		/* Calculating bar position and percentage */
		uint8_t BarPosition;
		float Percentage;
		
		BarPosition = (uint8_t)((Bar->BarSize * (Bar->CurrState - Bar->Min)) / (Bar->Max - Bar->Min));
		Percentage = 100.0 * ((float)(Bar->CurrState - Bar->Min) / (float)(Bar->Max - Bar->Min));
		
		/* Draw progress bar */
		drawing_bar(Graph, Bar->BarSize, BarPosition, Percentage, Bar->Precision);
		
		/* if is the last update, give one line space */
		if(Bar->CurrState == Bar->Max)
			printf("\n\n");
	}
}
/******************************************************************************/
void update_triple_bar(bar_t *Bar1, bar_graph_t *Graph1, int64_t CurrState1,
						bar_t *Bar2, bar_graph_t *Graph2, int64_t CurrState2,
						bar_t *Bar3, bar_graph_t *Graph3, int64_t CurrState3)
{
	static uint8_t NotUpdateFlag = 0;
	
	if(NotUpdateFlag)
		return;

	uint8_t Bar1Position, Bar2Position, Bar3Position;
	float Percentage1, Percentage2, Percentage3;
	
	Bar1->CurrState = CurrState1;
	Bar2->CurrState = CurrState2;
	Bar3->CurrState = CurrState3;

	if((Bar1->CurrState >= Bar1->TargetState) || (Bar1->CurrState == Bar1->Max))
	{
		/* Update next target state to redraw progress bar */
		if(Bar1->CurrState != Bar1->Max)
			Bar1->TargetState += Bar1->SliceSize;
		
		/* Calculating bar position and percentage */		
		Bar1Position = (uint8_t)((Bar1->BarSize * (Bar1->CurrState - Bar1->Min)) / (Bar1->Max - Bar1->Min));
		Percentage1 = 100.0 * ((float)(Bar1->CurrState - Bar1->Min) / (float)(Bar1->Max - Bar1->Min));
	}
	
	if((Bar2->CurrState >= Bar2->TargetState) || (Bar2->CurrState == Bar2->Max))
	{
		/* Update next target state to redraw progress bar */
		if(Bar2->CurrState != Bar2->Max)
			Bar2->TargetState += Bar2->SliceSize;
		
		/* Calculating bar position and percentage */		
		Bar2Position = (uint8_t)((Bar2->BarSize * (Bar2->CurrState - Bar2->Min)) / (Bar2->Max - Bar2->Min));
		Percentage2 = 100.0 * ((float)(Bar2->CurrState - Bar2->Min) / (float)(Bar2->Max - Bar2->Min));
	}
	
	if((Bar3->CurrState >= Bar3->TargetState) || (Bar3->CurrState == Bar3->Max))
	{
		/* Update next target state to redraw progress bar */
		if(Bar3->CurrState != Bar3->Max)
			Bar3->TargetState += Bar3->SliceSize;
		
		/* Calculating bar position and percentage */		
		Bar3Position = (uint8_t)((Bar3->BarSize * (Bar3->CurrState - Bar3->Min)) / (Bar3->Max - Bar3->Min));
		Percentage3 = 100.0 * ((float)(Bar3->CurrState - Bar3->Min) / (float)(Bar3->Max - Bar3->Min));
	}
	
	/* Draw triple progress bar */
	drawing_bar(Graph1, Bar1->BarSize, Bar1Position, Percentage1, Bar1->Precision);
	printf("\n");
	drawing_bar(Graph2, Bar2->BarSize, Bar2Position, Percentage2, Bar2->Precision);
	printf("\n");
	drawing_bar(Graph3, Bar3->BarSize, Bar3Position, Percentage3, Bar3->Precision);
	printf(ESC CSI LINEUP2);
	
	/* if is the last update, give one line space */
	if((Bar1->CurrState == Bar1->Max) && (Bar2->CurrState == Bar2->Max) && (Bar3->CurrState == Bar3->Max))
	{
		drawing_bar(Graph1, Bar1->BarSize, Bar1Position, Percentage1, Bar1->Precision);
		printf("\n");
		drawing_bar(Graph2, Bar2->BarSize, Bar2Position, Percentage2, Bar2->Precision);
		printf("\n");
		drawing_bar(Graph3, Bar3->BarSize, Bar3Position, Percentage3, Bar3->Precision);
		printf("\n\n");
		NotUpdateFlag = 1;
	}
}
/******************************************************************************/
void update_double_bar(bar_t *Bar1, bar_graph_t *Graph1, int64_t CurrState1,
						bar_t *Bar2, bar_graph_t *Graph2, int64_t CurrState2)
{
	static uint8_t NotUpdateFlag = 0;
	
	if(NotUpdateFlag)
		return;

	uint8_t Bar1Position, Bar2Position;
	float Percentage1, Percentage2;
	
	Bar1->CurrState = CurrState1;
	Bar2->CurrState = CurrState2;

	if((Bar1->CurrState >= Bar1->TargetState) || (Bar1->CurrState == Bar1->Max))
	{
		/* Update next target state to redraw progress bar */
		if(Bar1->CurrState != Bar1->Max)
			Bar1->TargetState += Bar1->SliceSize;
		
		/* Calculating bar position and percentage */		
		Bar1Position = (uint8_t)((Bar1->BarSize * (Bar1->CurrState - Bar1->Min)) / (Bar1->Max - Bar1->Min));
		Percentage1 = 100.0 * ((float)(Bar1->CurrState - Bar1->Min) / (float)(Bar1->Max - Bar1->Min));
	}
	
	if((Bar2->CurrState >= Bar2->TargetState) || (Bar2->CurrState == Bar2->Max))
	{
		/* Update next target state to redraw progress bar */
		if(Bar2->CurrState != Bar2->Max)
			Bar2->TargetState += Bar2->SliceSize;
		
		/* Calculating bar position and percentage */		
		Bar2Position = (uint8_t)((Bar2->BarSize * (Bar2->CurrState - Bar2->Min)) / (Bar2->Max - Bar2->Min));
		Percentage2 = 100.0 * ((float)(Bar2->CurrState - Bar2->Min) / (float)(Bar2->Max - Bar2->Min));
	}
	
	/* Draw triple progress bar */
	drawing_bar(Graph1, Bar1->BarSize, Bar1Position, Percentage1, Bar1->Precision);
	printf("\n");
	drawing_bar(Graph2, Bar2->BarSize, Bar2Position, Percentage2, Bar2->Precision);
	printf(ESC CSI LINEUP1);
	
	/* if is the last update, give one line space */
	if((Bar1->CurrState == Bar1->Max) && (Bar2->CurrState == Bar2->Max))
	{
		drawing_bar(Graph1, Bar1->BarSize, Bar1Position, Percentage1, Bar1->Precision);
		printf("\n");
		drawing_bar(Graph2, Bar2->BarSize, Bar2Position, Percentage2, Bar2->Precision);
		printf("\n\n");
		NotUpdateFlag = 1;
	}
}





