/****h* ScPovPlot3D/Balloons.inc
* PURPOSE
*   Macros for representation 3D scattered data as spheres proportional to value in specific space point
*     image:./imgs/BalloonsS.png
*     |html <hr width=50% align="left">
*   Fig.[Balloons] Representation of 3D scalar field V=V(x,y,z) by balloon's method 
*   |html <hr width=50% align="left">
*     *********************************************************
*     **   Tested on PovRay 3.7                              **
*     **   License: GNU GPL                                  **
*     **   Homepage:    http://scpovplot3d.sourceforge.net   **
*     *********************************************************
*     **   version: 3.1.1.1 (& have a nice time ;)             **
*     *********************************************************
* AUTHOR
*  Janusz Opi³a Ph.D.
*   jmo@agh.edu.pl, janusz.opila@gmail.com
*   Dept. of Applied Informatics
*   AGH University of Science & Technology, Cracow, Poland
*   Maintained by Janusz Opi³a Ph.D.
* COPYRIGHT
*  GNU GPL v.3 License
*  (c) 2012-now by Janusz Opi³a Ph.D.         
*  AGH University of Science and Technology
*
****
  end of RoboDoc comment
****/

#ifndef(Ballooon_INC)
#declare Ballooon_INC = "2014-05-25";
#version 3.7;
#declare _FileName   = "Balloon.INC";
#declare _FileVerMaj = "3.1.0.1";
#declare _FileVerMin = Ballooon_INC;
#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

#ifndef (Debug) // Just 4 safety
   #declare Debug=0;
#end

/*************************/
#declare BSceneMaxR = 0.0;  // extreme hypothetical values for balloon's radii, in this context possibly negative!
#declare BSceneMinR = 0.0;  // Min and Max

/****v* Balloons.inc/DeclareLevel
* PURPOSE
*   DeclareLevel determines what is defined:
*   * 0 = colour only
*   * 2 = also texture
*   * 4 = complete material
*   this value is valid until next "Set...()" declaration is met
* SOURCE 
*/
#declare DeclareLevel = 0;
/*******/
#declare tmpColour   = rgbf<0.2, 0.3, 0.4, 0.75>; //rgb <0, 0, 1>; // well, I like blue colour...
#declare tmpColour1  = rgbf<0.2, 0.3, 0.4, 0.75>; // ..transparent a little..
#declare tmpColour2  = rgbf<1.5, 1.3, 1.2, 0.73>;

/****f* Balloons.inc/SetColor()
* PURPOSE
*   Sets default color for different items:
*   - SetColor( color vector FullColour )
*   - SetRGBColor( float R, float G, float B) - for RGB definition style
*   - SetRGBFTColor( float R, float G, float B, float F, float T) for RGBFT definition
*   - SetRGBFTColor_1( float R, float G, float B, float F, float T) and
*   - SetRGBFTColor_2( float R, float G, float B, float F, float T)  two-color RGBFT items
****/
#macro SetRGBFTColor( RR, GG, BB, FF, TTT )
    #declare tmpColour = rgbft<RR, GG, BB, FF, TTT>;
    #declare DeclareLevel = 0;
#end

/****f* Balloons.inc/SetTexture()
* PURPOSE
*   declares texure for the item
* SYNOPSIS
SetTexture(texturedef)
****/
#macro SetTexture( FullTexture ) // follow it by SetInterior("InteriorDefinition")
    #declare tmpTexture = FullTexture ;
    #declare DeclareLevel = 2;
#end

/****f* Balloons.inc/SetInterior()
* PURPOSE
*   declares texure for the item
* SYNOPSIS
SetInterior(interiordef)
****/
#macro SetInterior( tInt )       // valid only together with texture defintions
    #declare tmpInterior = tInt;
    #declare DeclareLevel = 2;
#end

/****v* Balloons.inc/DZero
* PURPOSE               
*  minimal minimal float number assumed not beiing equal zero
*  application of this value depends on algorithm employed
* SOURCE 
*/
#declare DZero = 1E-4;    
/*******/

/****f* Balloons.inc/SetDZero()
* PURPOSE
*   Sets value of DZerovariable
****/
#macro SetDZero(dz)
   #declare DZero = dz;
#end

/****v* Balloons.inc/VScale
* PURPOSE               
*  minimal distance between grid & random point, 
*  this value is valid until "SetDzero()"
* SOURCE 
*/
#declare VScale = 1.0;    // Enhancement along vertical axis (negative values allowed ;)
/*******/

/****f* Balloons.inc/SetVScale()
* PURPOSE
*   Sets value of VScale variable
****/
#macro SetVScale(ss)
    #declare VScale=ss;   // Surface enhancement along vertical axis (negative values allowed ;)
#end
/*************************/                           

/****M* Balloons.inc/SphereOnColumn()
* PURPOSE
*   Draws sphere on top o column (or bottom if below zero XY plane )
* SEE ALSO
*   Nothing yet
* SYNOPSIS
SphereOnColumn(3Dvector Center, float Rs, float Rc)
* INPUTS
*   Center - center of the sphere in POVRay coordinates
*   Rs     - radius of the sphere
*   Rc     - radius of the cylinder 
****/
#macro SphereOnColumn(Center, Rs, rc)
    #merge{
           sphere{ Center, Rs }       
           #if (rc>Tiny)
                cylinder {<Center.x, 0, Center.z>,<Center.x, Center.y, Center.z>, rc}
           #else
                #debug "Base column diameter set to ZERO, base is discarded! \n"     
           #end
    }
#end

/****f* Balloons.inc/ScalingFunction()
* PURPOSE
*  Inline function scaling radius of sphere from real value V(x,y,z) to fit it in a reasonable range
* SEE ALSO
*  ScatterBalloonData(), "function()" definition in POVRay manual, function pointers
* INPUTS
*  this function takes one float argument 
* EXAMPLE
*/
#declare ScalingFunction  = function (rr) { 5*ln(abs(rr))+0.1 }   // default function - one may declare his own
/****/

/****M* Balloons.inc/ScatterBalloonData()
* PURPOSE
*  this macro creates chart for data in the form of z=f(x1, x2, x3) where z is marked as the radius of the sphere, uses SphereOnColumn() macro 
* SEE ALSO
*  SphereOnColumn(), ScalingFunction()
* SYNOPSIS
ScatterBalloonData(string filename InFileName, float er0, pfunction sigma, float columnRad) 
* INPUTS
*  InFileName - Data file name - for format see example 'Baloons.dat'
*  er0        - minimum radius of spheres, base radius
*  sigma      - pointer to inline function taking one float returning float, its goal is to scale radii to reasonable range
*  columnRad  - radius of column
* EXAMPLE
ScatterBalloonData("Balloons.dat", .710, ScalingFunction, 0.05)       
****/
#macro ScatterBalloonData(InFileName, er0, sigma, columnRad) // er0 = radius parameter, sigma(r) = radius function of sphere,
                                                             // columnRad = base column radius, none if EQ. zero
// Now open data file and import all points
   #fopen DataFile InFileName read
   #read(DataFile, DescriptX, DescriptY, DescriptZ)
   #read(DataFile, NumPts)
   #declare RNodes = array[NumPts][4];    // float vector: radius of nodes AND x, y, z
   #declare RNodes[0][0] = 0.0;  //x1
   #declare RNodes[0][1] = 0.0;  //x2
   #declare RNodes[0][2] = 0.0;  //x3
   #declare RNodes[0][3] = 0.0;  // f(x1,x2,x3)

#debug concat("Reading data from [", InFileName, "] file.\n")
   #declare rix=0;
   #while(rix < NumPts)
      #read(DataFile, xx, yy, zz, rr)
      // real world coords :
      #declare RNodes[rix][0] = xx;//x1
      #declare RNodes[rix][1] = yy;//x2
      #declare RNodes[rix][2] = zz;//x3
      #declare RNodes[rix][3] = rr;// f(x1,x2,x3)
   #debug concat("X[i]=<", str(rix, 4, 0),"=> ", str( RNodes[rix][0], 7, 4), ", ", str( RNodes[rix][1], 7, 4), ", ", str( RNodes[rix][2], 7, 4), ", ", str( RNodes[rix][3], 7, 4), ">\n")
      //===========
      #declare rix = rix + 1;
   #end

   #fclose DataFile
//==========================//

// I think 'x' and 'y' are exchanged due to POvRay Coord System
   #declare SceneMaxX =  RNodes[0][1];   #declare SceneMinX = SceneMaxX;
   #declare SceneMaxY =  RNodes[0][2];   #declare SceneMinY = SceneMaxY;
   #declare SceneMaxZ = -RNodes[0][0];   #declare SceneMinZ = SceneMaxZ;
   #declare BSceneMinR=  RNodes[0][3];   #declare BSceneMinR= BSceneMinR;

   #for(rix, 0, NumPts-1)
   // now PovRay crazy coordinates
         #declare xx = RNodes[rix][0];
         #declare yy = RNodes[rix][1];
         #declare zz = RNodes[rix][2];
         #declare rr = RNodes[rix][3];
#ifdef (Debug)  // use Declare=Debug=1 on command line to see this message
   #if (Debug>0)
      #debug concat("X[i]=<", str(rix, 4, 0),"' ", str( RNodes[rix][0], 7, 4), ", ", str( RNodes[rix][1], 7, 4), ", ", str( RNodes[rix][2], 7, 4), ", ", str( RNodes[rix][3], 7, 4), ">\n")
   #end
#end

         #if (zz+rr > SceneMaxY)
            #declare SceneMaxY=zz+rr;
         #end
         #if (zz-rr < SceneMinY)
            #declare SceneMinY=zz-rr;
         #end

         #if (-xx < SceneMaxZ)
            #declare SceneMaxZ=-xx;
         #end
         #if (-xx < SceneMinZ)
            #declare SceneMinZ=-xx;
         #end

         #if (yy > SceneMaxX)
            #declare SceneMaxX=yy;
         #end
         #if (yy < SceneMinX)
            #declare SceneMinX=yy;
         #end

         #if (rr > BSceneMaxR)
            #declare BSceneMaxR=rr;
         #end

         #if (rr < BSceneMinR)
            #declare BSceneMinR=rr;
         #end
     //===================
   #end
   #declare SceneMaxY=VScale*SceneMaxY;
   #declare SceneMinY=VScale*SceneMinY;

   #declare ZCoeff=(SceneMaxY>SceneMinY?1.0/(SceneMaxY-SceneMinY):1.0); // to avoid division by zero for flat surfaces..

   #for(rix, 0, NumPts-1)
       #ifdef (Debug)
         #if (Debug>0)
            #debug concat(" R(i) = ",  str(sigma(RNodes[rix][3]+er0), 19, 17),"\n")
         #end
       #end
       SphereOnColumn(<RNodes[rix][1], VScale*RNodes[rix][2], -RNodes[rix][0]>, sigma(RNodes[rix][3]+er0), columnRad)
   #end
// ready!
#end

#end // #ifdef