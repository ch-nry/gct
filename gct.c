/*  
	gct stand for Gcode tool : it allow to change the scale and offset of a 
	Gcode project.
	
    Copyright (C) 2023 Cyrille Henry

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/    

// version 1.0

// compilation :
// gcc -o gct gct.c -lm

// usage : 
// gct file.gcode newfile.gcode [arguments]
/* argument : 
    ScaleX float(x) : rescale the X values by a factor of x
    ScaleY float(x) : rescale the Y values by a factor of x
    ScaleZ float(x) : rescale the Z values by a factor of x
    OffsetX float(x) : add an offset of value x to all X coordinate
    OffsetY float(x) : add an offset of value x to all Y coordinate
    OffsetZ float(x) : add an offset of value x to all Z coordinate
    OffsetX : remove the offset of the X axes so the minimum X value is O
    OffsetY : remove the offset of the Y axes so the minimum Y value is O
    OffsetZ : remove the offset of the Z axes so the minimum Z value is O
    SizeX float(x): rescale the X values by a factor of x/Xmax, so the maximum X size is x
    SizeY float(x): rescale the Y values by a factor of x/Ymax, so the maximum Y size is x
    SizeZ float(x): rescale the Z values by a factor of x/Zmax, so the maximum Z size is x  
*/    
//multiples arguments is possible but they can interfer with each other     
// example : ./gct oldfile.gcode newfile.gcode offsetX 2 sizeX 20 
// will result of the X axes to be scaled between 2 and 22

#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000

float getnumber(char* line, int* offset) { // analyse a string starting at 'offset' and return a nupmber
// TODO : find a better way to do this
	char number[MAX_LINE_LENGTH];
	int i = 0;
	float x;
	while ( (line[i+*offset] != ' ')  && (line[i+*offset] != ';')  && (line[i+*offset] != '\n') ) { 
	// on recopie tout les digit, 1 par 1, jusqu'a tomber sur un espace, ; ou une fin de ligne
		number[i] = line[i+*offset]; 
		i++;
	}
	 number[i] = '\0'; // on termine la string
	 sscanf(number, "%f", &x);
	 *offset += i;
	 return x;
}

int main(int argc, char** argv) {
	FILE    *textfile, *newtextfile;
	char    line[MAX_LINE_LENGTH];
	int i;
	float x;
	float Xmin, Ymin, Xmax, Ymax, Zmin, Zmax; // curent min and max of the 3 axes
	float OX, OY, OZ, SX, SY, SZ;// offset and scale correction of the 3 axes
	Xmin = 100000;
	Ymin = 100000;
	Zmin = 100000;
	Xmax = -100000;
	Ymax = -100000;
	Zmax = -100000;
	OX = 0;
	OY = 0;
	OZ = 0;
	SX = 1;
	SY = 1;
	SZ = 1;
	
	if ( argc < 2 ) {
		printf("1st argument should be the Gcode file to convert\n");
		return 1;
	}

	textfile = fopen(argv[1], "r");
	if(textfile == NULL) {
		printf("can not read file\n");
		return 1;
	}
	
	while(fgets(line, MAX_LINE_LENGTH, textfile)){ // for all lines
		if((line[0] == 'G') && (line[1] == '1'))  {
			//printf(line);  // la ligne  commence par G1
			i=3;
			while ( (line[i] != ';')  && (line[i] != '\n') ) {
				switch (line[i]) {
				case 'X':
					i++;
					x = getnumber(line, &i);
					//printf("%f ",x);
					if(Xmin > x) Xmin = x; // on prend le minimum
					if(Xmax < x) Xmax = x; 
				break;
				case 'Y':
					i++;
					x = getnumber(line, &i);
					//printf("%f ",x);
					if(Ymin > x) Ymin = x; // on prend le minimum
					if(Ymax < x) Ymax = x; // on prend le minimum
				break;
				case 'Z':
					i++;
					x = getnumber(line, &i);
					//printf("%f ",x);
					if(Zmin > x) Zmin = x; // on prend le minimum
					if(Zmax < x) Zmax = x; // on prend le minimum
				break;
				default:
					i++;
			    }
		    }
	    }
	}
	fclose(textfile);
	printf("Xmin :%f Ymin: %f Zmin: %f Xmax: %f Ymax: %f Zmax: %f\n", Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);

//------------------------------------------------------------

	if ( argc <= 2 ) {
	    printf("use a 2nd argument to convert the Gcode\n");
		return 1; // missing 2nd argument, only analyse the gcode
	}
	if ( argc <= 3 ) {
	    printf("No modification will be apply\n");
	    printf("Add arguments to change the scale or the offset of the input file\n");
	    printf("Arguments are : \n");
	    printf("	ScaleX float, ScaleY float, ScaleZ float, to change the scale\n");
	    printf("	SizeX float, SizeY float, SizeZ float, to change the scale, roviding only the final size\n");
	    printf("	OffsetX float, OffsetY float, OffsetZ float, to add an offset (I.E move everything)\n");
	    printf("	OffsetX, OffsetY, OffsetZ, to remove the current offset (I.E having 0 as the minimum position)\n");
	}
    for (i = 3; i < argc; i++) { // get all other arguments
        if( (strcmp(argv[i], "ScaleX")==0) && (argc > (i+1))  && (sscanf(argv[i+1], "%f", &x)) ){
            SX = x;        
            printf("scaleX : %f\n", x);
            i++; // no need to test next argument for a keyword, since it's a float
            continue;
        }
        if( (strcmp(argv[i], "ScaleY")==0) && (argc > (i+1))  && (sscanf(argv[i+1], "%f", &x)) ){
            SY = x;        
            printf("scaleY : %f\n", x);
            i++; // no need to test next argument for a keyword, since it's a float
            continue;
        }
        if( (strcmp(argv[i], "ScaleZ")==0) && (argc > (i+1))  && (sscanf(argv[i+1], "%f", &x)) ){
            SZ = x;        
            printf("scaleZ : %f\n", x);
            i++; // no need to test next argument for a keyword, since it's a float
            continue;
        }
        if( (strcmp(argv[i], "OffsetX")==0) && (argc > (i+1))  && (sscanf(argv[i+1], "%f", &x)) ){
            OX = x;        
            printf("offsetX : %f\n", x);
            i++; // no need to test next argument for a keyword, since it's a float
            continue;
        }
        if( (strcmp(argv[i], "OffsetY")==0) && (argc > (i+1))  && (sscanf(argv[i+1], "%f", &x)) ){
            OY = x;        
            printf("offsetY : %f\n", x);
            i++; // no need to test next argument for a keyword, since it's a float
            continue;
        }
        if( (strcmp(argv[i], "OffsetZ")==0) && (argc > (i+1))  && (sscanf(argv[i+1], "%f", &x)) ){
            OZ = x;        
            printf("offsetZ : %f\n", x);
            i++; // no need to test next argument for a keyword, since it's a float
            continue;
        }
        if( (strcmp(argv[i], "SizeX")==0) && (argc > (i+1))  && (sscanf(argv[i+1], "%f", &x)) ){
            SX = x/Xmax;        
            printf("scaleX : %f\n", SX);
            i++; // no need to test next argument for a keyword, since it's a float
            continue;
        }        
        if( (strcmp(argv[i], "SizeY")==0) && (argc > (i+1))  && (sscanf(argv[i+1], "%f", &x)) ){
            SY = x/Ymax;        
            printf("scaleY : %f\n", SY);
            i++; // no need to test next argument for a keyword, since it's a float
            continue;
        }        
        if( (strcmp(argv[i], "SizeZ")==0) && (argc > (i+1))  && (sscanf(argv[i+1], "%f", &x)) ){
            SZ = x/Zmax;        
            printf("scaleZ : %f\n", SZ);
            i++; // no need to test next argument for a keyword, since it's a float
            continue;
        }        
        if(strcmp(argv[i], "OffsetX")==0){
            OX = -Xmin;        
            printf("offsetX : %f\n", OX);
            continue;
        }
        if(strcmp(argv[i], "OffsetY")==0){
            OY = -Ymin;        
            printf("offsetY : %f\n", OY);
            continue;
        } 
        if(strcmp(argv[i], "OffsetZ")==0){
            OZ = -Zmin;        
            printf("offsetZ : %f\n", OZ);
            continue;
        }
    }

//------------------------------------------------------------

	textfile = fopen(argv[1], "r"); // open again the 1st file to read it from the beggining
	if(textfile == NULL) return 1;
	
	newtextfile = fopen(argv[2], "w"); // opening the new file
	if(newtextfile == NULL){
      printf("Error!, can not write file");   
      return 1;
   }
	
	while(fgets(line, MAX_LINE_LENGTH, textfile)){ // for all lines
		if( !((line[0] == 'G') && (line[1] == '1')) ) {
			fprintf(newtextfile, "%s", line); // the line did not start with G1 : we just copy without modification
		}
		else {// the line start with G1 : we analyse it and recreate it after coordinates moduifications
			fprintf(newtextfile, "G1 ");
			i=3;
			while (line[i] != '\n') {
				switch (line[i]) {
				case 'X':
					fprintf(newtextfile, "X");
					i++;
					x = getnumber(line, &i);
					fprintf(newtextfile, "%5.4f", OX + x*SX);
				break;
				case 'Y':
					fprintf(newtextfile, "Y");
					i++;
					x = getnumber(line, &i);
					fprintf(newtextfile, "%5.4f",OY + x*SY);
				break;
				case 'Z':
					fprintf(newtextfile, "Z");
					i++;
					x = getnumber(line, &i);
					fprintf(newtextfile, "%5.4f",OZ + x*SZ);
				break;
				default:
					fprintf(newtextfile, "%c",line[i]);
					i++;
				}
			}
			fprintf(newtextfile, "\n"); 
		}
	}
	fclose(textfile);
	fclose(newtextfile);
	printf("suceesfully write %s file with new coordinate : \n", argv[2]);   
    printf("Xmin :%f Ymin: %f Zmin: %f Xmax: %f Ymax: %f Zmax: %f\n", OX + Xmin*SX, OY + Ymin*SY, OZ + Zmin*SZ, OX + Xmax*SX, OY + Ymax*SY, OZ + Zmax*SZ);
    
	return 0;
}
