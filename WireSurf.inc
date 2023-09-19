/****h* ScPovPlot3D/WireSurf.inc
* PURPOSE
*   Macros for representation of 3D surfaces as wireframes
* DESCRIPTION 
*    image:./imgs/WireFramesFour.jpg
*
*   Fig.[WireFrame] Linear (twice), Cubic and BSpline interpolation
*   |html <hr width=50% align="left">
* SYNOPSIS
* *********************************************************
* **   Tested on PovRay 3.7                              **
* **   License: GNU GPL                                  **
* **   Homepage:    http://scpovplot3d.sourceforge.net   **
* *********************************************************
* **   version: 4.0.0.2 (& have a nice time ;)             **
* *********************************************************
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
* HISTORY 
* 2021-06-09/2 - correction of computing radius of the wires and nodes. Now looks identical in all styles.
* 2021-06-09   - b_spline{} sphere_sweep curated, now works as it should
*              b_spline is described in doc like cubic spline, in fact it is 
*              Bezier with two extra eksternal points.                 
* 
****
  end of RoboDoc comment
****/

#ifndef(WireSurf_INC)
#declare WireSurf_INC = "2021-06-10";
#version 3.7;

#declare _FileName   = input_file_name;
#declare _FileVerMaj = "4.0.0.2";
#declare _FileVerMin = WireSurf_INC;
#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

#ifndef (Debug) // Just 4 safety
   #declare Debug=0;
#end

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"
#include "glass.inc"
#include "math.inc"

// here are macros defining some useful cameras
/*********************/
#include "Cameras.inc"
#include "CommonDefs.inc"

#declare YScaleFactor = 1.0; // exaggerates vertical dimension
#macro SetYScaleFactor(YSF)
    #declare YScaleFactor = YSF;
#end
#macro AdjustMaxY(Y) // Overrides automatically evaluated number
    #declare DscMaxY = Y;
    #declare MaxY= DscMaxY*YScaleFactor;
    #declare SceneMaxY=MaxY;
#end

#declare tmpColour   = rgb <0,0,1>; // well, I like blue...
#declare tmpColour1  = rgbf<0.2, 0.3, 0.4, 0.1>;
#declare tmpColour2  = rgbf<1.5, 1.3, 1.2, 0.3>;
#declare tmpInterior = inGlass;
#declare tmpTexture  = txBlue;
#declare tmpMaterial = mtGlass;

#declare M_Glass=    // Glass material
material {
  texture {
    pigment {rgbt 1}
    finish {
      ambient 0.0
      diffuse 0.05
      specular 0.6
      roughness 0.005
      reflection {
        0.1, 1.0
        fresnel on
      }
      conserve_energy
    }
  }
  interior {
    ior 1.5
    fade_power 1001
    fade_distance 0.9
    fade_color <0.5,0.8,0.6>
  }
}

/****iv* WireSurf.inc/DeclareLevel
* PURPOSE
*   DeclareLevel variable determines what is defined:
*   * 0 = just colour,
*   * 2 = full texture item,
*   * 4 = complete material definition.
*   this value is valid until the next "Set...()" declaration is met
*   few more default internal values
* SOURCE 
*/
#declare DeclareLevel = 0;
#declare NodeRadiiCoeff=0.20;
#declare WireRadiiCoeff=NodeRadiiCoeff/sqrt(2);

#declare shiftXY    = -1.4;
#declare LettrSize  =  0.8;
#declare LettrDepth =  0.2;
#declare LettrBase  = -0.7;
#declare DscrAngle  =  0.0;
#declare NumDscrLgh =  5;
#declare NumDscrDig =  3;

#declare NRow = 1;
#declare NCol = 1;
/******/

/****f* WireSurf.inc/SetLettrSize()
* PURPOSE
*   Sets size of letters in description of axes
* SYNOPSIS
SetLettrSize(float ls)
*****/
#macro SetLettrSize(ls)
    #declare LettrSize = ls;
#end

/****f* WireSurf.inc/SetLettrDepth()
* PURPOSE
*   Sets size of letters in description of axes
* SYNOPSIS
SetLettrDepth(float ls)
******/             
#macro SetLettrDepth(ls)
    #declare LettrDepth= ls;
#end


/****f* WireSurf.inc/SetLettrBase()
* PURPOSE
*   Sets base position of letters in description of axes
* SYNOPSIS
SetLettrBase(float ls)
******/                  
#macro SetLettrBase(ls)
    #declare LettrBase =ls;
#end

/****f* WireSurf.inc/SetDscrAngle()
* PURPOSE
*   Sets initial angle of description of axes (category labels)
* SYNOPSIS
*/
#macro SetDscrAngle(_ls)
/*
* INPUTS
*   float _ls - contains finish {} section for background images
* SEE ALSO
******/
    #declare DscrAngle = _ls;
#end

/****f* WireSurf.inc/SetBgPicture()
* PURPOSE
*   Sets picture for the background of coords system
*   Will be moved to CoordsSys.inc
* SYNOPSIS               
*/
#macro SetBgPicture(_Pct)
/*
* INPUTS
*   string_fn Pct
******/
  #declare BgPicture = _Pct;
#end // macro SetBgPicture()

/****f* WireSurf.inc/TurnPhotonsDown(), TurnPhotonsUp()
* PURPOSE
*   Sets internal variable Photons to 'no' or 'yes'
*   ie turns photon mechanism 'off' or 'on'
* SYNOPSIS
TurnPhotonsUp()
TurnPhotonsDown()
******/
#macro TurnPhotonsDown()
    #declare Photons=false;
#end

#macro TurnPhotonsUp()
    #declare Photons=true;
#end

#macro SetEffectsOn()
    #declare Effects=true;
#end

#macro SetEffectsOff()
    #declare Effects=false;
#end

/****f* WireSurf.inc/SetColor()
* PURPOSE
*   Sets default color for different items:
*   - SetColor( color vector FullColour )
*   - SetRGBColor( float R, float G, float B) - for RGB definition style
*   - SetRGBFTColor( float R, float G, float B, float F, float T) for RGBFT definition
*   - SetRGBFTColor_1( float R, float G, float B, float F, float T) and
*   - SetRGBFTColor_2( float R, float G, float B, float F, float T)  two-color RGBFT items
****/
#macro SetColor( FullColour )
    #declare tmpColour = FullColour ;
    #declare DeclareLevel = 0;
#end

#macro SetRGBColor(R, G, BB)
    #declare tmpColour = rgb<R, G, BB >;
    #declare DeclareLevel = 0;
#end

#macro SetRGBFTColor( RR, GG, BB, FF, TTT )
    #declare tmpColour = rgbft<RR, GG, BB, FF, TTT>;
    #declare DeclareLevel = 0;
#end

#macro SetRGBFTColor_1( RR, GG, BB, FF, TTT ) // for hollow bar
    #declare tmpColour1 = rgbft<RR, GG, BB, FF, TTT>;
    #declare DeclareLevel = 0;
#end

#macro SetRGBFTColor_2( RR, GG, BB, FF, TTT ) // for hollow bar
    #declare tmpColour2 = rgbft<RR, GG, BB, FF, TTT>;
    #declare DeclareLevel = 0;
#end

/****f* WireSurf.inc/SetTexture()
* PURPOSE
*   declares texure for the item
* SYNOPSIS
SetTexture( texturedef FullTexture )
****/
#macro SetTexture( FullTexture ) // follow it by SetInterior("InteriorDefinition")
    #declare tmpTexture = FullTexture ;
    #declare DeclareLevel = 2;
#end

/****f* WireSurf.inc/SetInterior()
* PURPOSE
*   declares texure for the item
* SYNOPSIS
SetInterior( interiordef tInt )
****/
#macro SetInterior( tInt ) // valid only together with texture defintions
    #declare tmpInterior = tInt;
    #declare DeclareLevel = 2;
#end

/****f* WireSurf.inc/SetMaterial()
* PURPOSE
*   declares material for the item
* SYNOPSIS
SetMaterial( materialdef FullMaterial )
****/
#macro SetMaterial( FullMaterial )
    #declare tmpMaterial = FullMaterial;
    #declare DeclareLevel = 4;
#end

/****f* WireSurf.inc/SetNodeRadiiCoeff(Coeff)
* PURPOSE
*   declares radius coefficient for the wire and nodes
* SYNOPSIS
SetNodeRadiiCoeff(float Coeff)
****/
#macro SetNodeRadiiCoeff(_Coeff)
  #declare NodeRadiiCoeff= (_Coeff>Tiny?_Coeff:Tiny);  // adjustment of node radii
//  #declare WireRadiiCoeff=NodeRadiiCoeff/sqrt(2);
#end

/****f* WireSurf.inc/SetWireRadiiCoeff ( _Coeff )
* PURPOSE
*   declares radius coefficient for the wire only
* SYNOPSIS
SetWireRadiiCoeff(float _Coeff)
****/
#macro SetWireRadiiCoeff(_Coeff)
  #declare WireRadiiCoeff= (_Coeff>Tiny?_Coeff:Tiny);  // adjustment of bar radii

#end

/**************************************/
#macro SetNumFormat(L, D)
    #declare NumDscrLgh=L;
    #declare NumDscrDig=D;
#end
#macro SetXDscOffset(Off)
   #declare shiftXY=Off;
#end

/****f* WireSurf.inc/DrawVrtDescr()
* PURPOSE
*   Draws automatically vertical axis description,
* SYNOPSIS
DrawVrtDescr(uinteger _NVrt, texturedef _Texture)
* INPUTS
*   _NVrt - number of major tics
*   _Texture - texture identifier
****/
#macro DrawVrtDescr(_NVrt, _Texture)
  #declare _NVrt=_NVrt+1;
  #declare DscrVrtAxe = array[_NVrt];     // description along Y-axis (ie vertical axe)
  #if (_NVrt>1)
    #declare VrtDlt = DscMaxY/(_NVrt-1);
    #declare Cnt=0;
    #while (Cnt<_NVrt)
       #declare DscrVrtAxe[Cnt]= Cnt*VrtDlt;
       #declare Cnt=Cnt+1;
    #end

    #declare Cnt=0;
    #while (Cnt<_NVrt)   // Y-axe AKA Cartesian 'Z'
      text{
         ttf "Arial.ttf",str( DscrVrtAxe[Cnt], NumDscrLgh, NumDscrDig), LettrDepth, 0 texture{_Texture}
         scale LettrSize
         rotate DscrAngle*y
         translate<shiftXY, YScaleFactor*(.97*DscrVrtAxe[Cnt]), -1.05*BaseZL>
      }
    #declare Cnt=Cnt+1;
    #end

  #end //#if
#end

/****f* WireSurf.inc/InitDscrColAxe()
* PURPOSE
*   Initializes description data block along horizontal axis (columns)
* SYNOPSIS
InitDscrColAxe(uinteger N)
* INPUTS
*   N - number of categories
****/
#macro InitDscrColAxe(N)
    #declare DscrColAxe = array[N];       // description along X-axis (ie column axe)
    #declare NCol=N;
#end
/**************************************/

/****f* WireSurf.inc/AddColDescr()
* PURPOSE
*   Inserts into horizontal axis data block new major label
* SYNOPSIS
AddColDescr(string Dscr)
* INPUTS
*   Dscr - label ("1", "A" or so on..)
****/
#macro AddColDescr(Dscr)
  #ifndef (ColCnt)
    #declare ColCnt=0;
  #else
    #declare ColCnt=ColCnt+1;
  #end

  #if (ColCnt<NCol)
     #declare DscrColAxe[ColCnt] = Dscr;
  #else
    #warning concat("==>Too many descriptions for cols: \"", Dscr, "\" rejected\n")
  #end
#end
/**************************************/
//Znn

/****f* WireSurf.inc/DrawColDscr()
* PURPOSE
*   Inserts into rendering stack description of horizontal axis
* SYNOPSIS
DrawColDscr(texturedef _Txtr, float _LettrBase, float _LettrSize, float _LettrDepth)
* INPUTS
*   _Txtr       - texture definition
*   _LettrBase  - adjustment to the position of description
*   _LettrSize  - Size of characters in description text
*   _LettrDepth - thickness of characters in description text
****/
#macro DrawColDscr(_Txtr, _LettrBase, _LettrSize, _LettrDepth) // Texture, Letter base
  #declare xPos=shiftXY;
  #declare Cnt=0;
  #while(Cnt<NCol)
    text{
       ttf "Arial.ttf", DscrColAxe[Cnt], _LettrDepth, 0 texture{_Txtr}
       scale _LettrSize
       rotate DscrAngle*y
       translate<xPos, _LettrBase, -1.05*BaseZL>
    }
    #declare xPos=xPos+deltaX;
    #declare Cnt=Cnt+1;
  #end
#end
/**************************************/

/****f* WireSurf.inc/InitDscrRowAxe()
* PURPOSE
*   Initializes description data block along horizontal axis (rows)
* SYNOPSIS
InitDscrRowAxe(uinteger N)
* INPUTS
*   N - number of categories
****/
#macro InitDscrRowAxe(N)
    #declare DscrRowAxe = array[N];     // description along Z-axis (ie row axe)
    #declare NRow = N;     // description along Z-axis (ie row axe)
#end

/****f* WireSurf.inc/AddRowDescr()
* PURPOSE
*   Inserts into 2nd horizontal axis data block new major label
* SYNOPSIS
AddRowDescr(string Dscr)
* INPUTS
*   Dscr - label ("1", "A" or so on..)
****/
#macro AddRowDescr(Dscr)
  #ifndef (RowCnt)
    #declare RowCnt=0;
  #else
    #declare RowCnt=RowCnt+1;
  #end
  #if (RowCnt<NRow)
    #declare DscrRowAxe[RowCnt] = Dscr;
  #else
    #warning concat("==>Too many descriptions for rows: \"", Dscr, "\" rejected\n")
  #end
#end
/**************************************/

/****f* WireSurf.inc/DrawRowDscr()
* PURPOSE
*   Inserts into rendering stack description of 2nd horizontal axis
* SYNOPSIS
DrawRowDscr(texturedef _Txtr, float _LettrBase, float _LettrSize, float _LettrDepth)
* INPUTS
*   _Txtr       - texture definition
*   _LettrBase  - adjustment to the position of description
*   _LettrSize  - Size of characters in description text
*   _LettrDepth - thickness of characters in description text
****/
#macro DrawRowDscr(_Txtr, _LettrBase, _LettrSize, _LettrDepth) // Texture, Letter base
  #declare ZPos = 0.0;
  #declare Cnt=0;
  #while(Cnt<NRow)
    text{
       ttf "Arial.ttf", DscrRowAxe[Cnt], _LettrDepth,0 texture{_Txtr}
       scale _LettrSize
       rotate DscrAngle*y
       translate<MaxX+shiftXY, _LettrBase, ZPos>
    }

    #declare ZPos=ZPos-deltaY;
    #declare Cnt=Cnt+1;
  #end
  #declare NRow=1;
#end
/**************************************/

/****M* WireSurf.inc/DrawWireFrameRC(), DrawWireFrame()
* PURPOSE
*   Draws representation od data from input file as wireframed 3D surface.
*   Input data is organized as a matrix of floats separated by commas.
*   This macro is intendend for visualisation of matrices thus "RC" suffix (Row/Column).
*   macro DrawWireFrame(DataFileName) is just backward compatibility wrapper
*   SEE ALSO
*   DrawWireFrameYX()
* SYNOPSIS
DrawWireFrameRC(string filename DataFileName)
* INPUTS
*   DataFileName - valid name of file with data - see example file for details
*   of its structure. Generally speaking first is Column axis title, Row Yaxis title,
*   number of columns, then min and max value assigned to the first and last item in row,
*   number of rows, then min and max value assigned to the first and last item in column,
*   eventually numeric data in form of matrix, ie. data measured, observed or
*   calculated over the grid
* SOURCE
/// #macro DrawWireFrameRC(DataFileName) is intendend for visualisation of matrices
/// interpretation of structure of input file (r=Row, c=Column):
///  * -----> Column ---->
///  |  r1c1 r1c2 r1c3 r1c3
///  |  r2c1
///  |  r3c1...
///  V  r4c1..
/// Rows
///  ie. rows are treated as rows of the matrix so they are drawn
///  along PovRAY X  axis (which is Y in REAL coords).
****/
#macro DrawWireFrame(DataFileName)     // obsolete - backward compatibility wrapper
   DrawWireFrameRC(DataFileName)
#end
#macro DrawWireFrameRC(DataFileName)       // first row of grid is drawn along PovRAY X axis
    #fopen DataFile DataFileName read
    #read(DataFile, DescriptX, DescriptY)
    #read(DataFile, Nx, x1, x2)
    #read(DataFile, Ny, y1, y2)
    #if ((Nx>1)&(Ny>1))

        #declare Nodes=array[Ny][Nx];    // float: height of nodes only

        #declare deltaX=(x2-x1)/(Nx-1);
        #declare deltaY=(y2-y1)/(Ny-1);
        #declare QSurfSide=sqrt(deltaX*deltaY);

        #declare Nodes[0][0]=0.0;        // explicit typing array as float

        #declare MinY = 1E22;
        #declare MaxY =-1E22;

        #declare MinX=x1;
        #declare MaxX=x2;

        #declare MinZ=-y1;
        #declare MaxZ=-y2;

        #declare BaseZL=y2;
        #declare NRow=1;

        #declare NodeRadii=NodeRadiiCoeff*QSurfSide;
        #declare WireRadii=WireRadiiCoeff*QSurfSide;

        #declare CountY=0;
        #while(CountY<Ny)
          #declare CountX=0;
          #while(CountX<Nx)
               #read(DataFile, Nodes[CountY][ CountX])

               #if (Nodes[CountY][CountX]<MinY)
                 #declare MinY=Nodes[CountY][CountX];
               #end

               #if (Nodes[CountY][CountX]>MaxY)
                 #declare MaxY=Nodes[CountY][CountX];
               #end
             #declare CountX=CountX+1;
          #end
          #declare CountY=CountY+1;
        #end

        #fclose DataFile

        #debug concat("MaxY= ",str(MaxY,10,7), "\n********\n\n")

        #declare DscMaxY=MaxY;
        #declare MaxY=YScaleFactor*MaxY;
        #declare SceneMaxY=MaxY;
        #declare SceneMinX=x1;
        #declare SceneMaxX=x2;
        #declare SceneMinZ=-y1;
        #declare SceneMaxZ=-y2;

        #declare CountY=0;
        #union{
            #while(CountY<Ny)
              #declare CountX=0;
              #while(CountX<Nx)
                #declare YYY=YScaleFactor*Nodes[CountY][CountX];
/*
                sphere {
                      <CountX*deltaX, YYY, -CountY*deltaY>, NodeRadii

                      material {
                      texture {
                        pigment {rgbt <Nodes[CountY][CountX]/DscMaxY,.0,1-Nodes[CountY][CountX]/DscMaxY,  0.3>}
                        finish {
                          ambient 2.0
                          diffuse 0.05
                          specular 0.6
                          roughness 0.005
                          reflection {
                            0.1, 1.0
                            fresnel on
                          }
                          //conserve_energy
                        }
                      }
                      interior {
                        ior 1.5
                        fade_power 1001
                        fade_distance 0.9
                        fade_color <Nodes[CountY][CountX]/DscMaxY,.5,1-Nodes[CountY][CountX]/DscMaxY>
                      }
                    }
                  #if (Photons)      // if photons set on...
                   photons {         // photon block for an object
                    target 1.0
                    refraction on
                    reflection on
                  }
                  #end
                }
*/                

                #if ((CountX+1<Nx) &(CountY<Ny))
                    #declare YYY2=YScaleFactor*Nodes[CountY][CountX+1];
                    merge{
                        sphere{<    CountX*deltaX,  YYY, -CountY*deltaY>, WireRadii}
                        cylinder{
                              <    CountX*deltaX,  YYY, -CountY*deltaY>,
                              <(CountX+1)*deltaX, YYY2, -CountY*deltaY>, WireRadii
                        }
                        sphere{<(CountX+1)*deltaX, YYY2, -CountY*deltaY>, WireRadii}

                        texture{ Chrome_Metal
                               finish { diffuse 0.9 phong 1 }
                               scale 0.5 translate<0,1,0>
                        } // end of texture ---------------------------

                    } // end merge{}
                #end

                #if ((CountX<Nx) &(CountY+1<Ny))
                    #declare YYY2=YScaleFactor*Nodes[CountY+1][CountX];
                    merge{
                        sphere{<CountX*deltaX,  YYY,     -CountY*deltaY>, WireRadii}
                        cylinder{
                              <CountX*deltaX,  YYY,     -CountY*deltaY>,
                              <CountX*deltaX, YYY2, -(CountY+1)*deltaY>, WireRadii
                        }
                        sphere{<CountX*deltaX, YYY2, -(CountY+1)*deltaY>, WireRadii}
                        texture{ Chrome_Metal
                                finish { diffuse 0.9 phong 1 }
                                scale 0.5 translate<0,1,0>
                        } // end of texture ---------------------------


                    } // end merge{}
                #end
                 #declare CountX=CountX+1;
              #end
              #declare CountY=CountY+1;
            #end
        } // union
        #declare ShiftXY=-0.05*(SceneMinX+SceneMaxX)/2;
    #else
      #debug "* * * * * * * * * * * * * * * * * * * * * * * * *\n"
      #debug "* To few nodes.  You need at least 2x2=4 nodes! *\n"
      #debug "* * * * * * * * * * * * * * * * * * * * * * * * *\n"
    #end
#end // #macro DrawWireFrame()


/****M* WireSurf.inc/DrawWireFrameXY()
* PURPOSE
*   Draws representation od data from input file as wireframed 3D surface.
*   Input data is organized as a matrix of floats separated by commas.
*   This macro is intendend for visualisation of gridded data thus "YX" suffix.
* SYNOPSIS
DrawWireFrameXY(string filename DataFileName)
* INPUTS
*   DataFileName - valid name of file with data - see example file for details
*   of its structure. Generally speaking first is Xaxis title, Yaxis title,
*   number of columns, then min and max value assigned to the items in first and last X column respectively (along POVRay X axis),
*   number of rows, then min and max value assigned to the items in first and last row respectively (along POVRay -Z axis),
*   eventually numeric data in form of matrix, ie. data measured, observed or
*   calculated over the real XY grid
* SOURCE
/// interpretation of structure of input file:
///  * -----> X ---->
///  |  x1y1 x1y2 x1y3 x1y3
///  |  x2y1
/// Y|  x3y1...
///  V  X4y1..
///  ie. rows are treated as x-data so they are drawn
///  along REAL X  axis (which is -Z in PovRAY coords) ,
****
/// #macro DrawWireFrameXY(DataFileName)
*/
#macro DrawWireFrameXY(DataFileName) // first row is drawn along Real X axis
    #fopen DataFile DataFileName read
    #read(DataFile, DescriptX, DescriptY)
    #read(DataFile, Nx, x1, x2)
    #read(DataFile, Ny, y1, y2)
    #if ((Nx>1)&(Ny>1))

        #declare Nodes=array[Ny][Nx];          // float: height of nodes only

        #declare deltaX=(x2-x1)/(Nx-1);
        #declare deltaY=(y2-y1)/(Ny-1);
        #declare QSurfSide=sqrt(deltaX*deltaY);

        #declare Nodes[0][0]=0.0;                       // explicit typing array as float

        #declare MinY = 1E22;
        #declare MaxY =-1E22;

        #declare MinX=x1;
        #declare MaxX=x2;

        #declare MinZ=-y1;
        #declare MaxZ=-y2;

        #declare BaseZL=y2;
        #declare NRow=1;

        #declare NodeRadii=NodeRadiiCoeff*QSurfSide;
        #declare WireRadii=WireRadiiCoeff*QSurfSide;
// #debug concat("QSurf=", str(QSurfSide,10,7), "; NodeR=", str(NodeRadii,10,7), "; Wire radii=", str(WireRadii,10,7), "\n")

        #declare CountY=0;
        #while(CountY<Ny)
          #declare CountX=0;
          #while(CountX<Nx)
               #read(DataFile, Nodes[CountY][ CountX])

               #if (Nodes[CountY][CountX]<MinY)
                 #declare MinY=Nodes[CountY][CountX];
               #end

               #if (Nodes[CountY][CountX]>MaxY)
                 #declare MaxY=Nodes[CountY][CountX];
               #end
             #declare CountX=CountX+1;
          #end
          #declare CountY=CountY+1;
        #end

        #fclose DataFile

        #debug concat("MaxY= ",str(MaxY,10,7), "\n********\n\n")

        #declare DscMaxY=MaxY;
        #declare MaxY=YScaleFactor*MaxY;
        #declare SceneMaxY=MaxY;
        #declare SceneMinX=x1;
        #declare SceneMaxX=x2;
        #declare SceneMinZ=-y1;
        #declare SceneMaxZ=-y2;

        #declare CountY=0;
        #union{
            #while(CountY<Ny)
              #declare CountX=0;
              #while(CountX<Nx)
                 #declare YYY=YScaleFactor*Nodes[CountY][CountX];
/*
                 sphere {
                      <CountY*deltaY, YYY, -CountX*deltaX>, NodeRadii
                      material {
                      texture {
                        pigment {rgbt <Nodes[CountY][CountX]/DscMaxY,.0,1-Nodes[CountY][CountX]/DscMaxY,  0.3>}
                        finish {
                          ambient 2.0
                          diffuse 0.05
                          specular 0.6
                          roughness 0.005
                          reflection {
                            0.1, 1.0
                            fresnel on
                          }
                          //conserve_energy
                        }
                      }
                      interior {
                        ior 1.5
                        fade_power 1001
                        fade_distance 0.9
                        fade_color <Nodes[CountY][CountX]/DscMaxY,.5,1-Nodes[CountY][CountX]/DscMaxY>
                      }
                    }
                  #if (Photons)      // if photons set on...
                   photons {         // photon block for an object
                    target 1.0
                    refraction on
                    reflection on
                  }
                  #end
                }
*/
                #if ((CountX+1<Nx) &(CountY<Ny))
                      #declare YYY2=YScaleFactor*Nodes[CountY][CountX+1];
                      merge{
                          sphere{<CountY*deltaY, YYY, -CountX*deltaX>, WireRadii}
                          cylinder{
                              <CountY*deltaY, YYY, -CountX*deltaX>,
                              <CountY*deltaY, YYY2, -(CountX+1)*deltaX>,  WireRadii
                          }
                          sphere{<CountY*deltaY, YYY2, -(CountX+1)*deltaX>, WireRadii}
                    }
                #end

                #if ((CountX<Nx) &(CountY+1<Ny))
                    #declare YYY2=YScaleFactor*Nodes[CountY+1][CountX];
                    merge{
                        sphere{<CountY*deltaY, YYY, -CountX*deltaX>, WireRadii}
                        cylinder{
                              <CountY*deltaY, YYY, -CountX*deltaX>,
                              <(CountY+1)*deltaY, YYY2, -CountX*deltaX>, WireRadii
                        }
                        sphere{<(CountY+1)*deltaY, YYY2, -CountX*deltaX>, WireRadii}
                    }
                #end
                 #declare CountX=CountX+1;
              #end
              #declare CountY=CountY+1;
            #end
        } // union
        #declare ShiftXY=-0.05*(SceneMinX+SceneMaxX)/2;
    #else
      #debug "* * * * * * * * * * * * * * * * * * * * * * * * *\n"
      #debug "* To few nodes.  You need at least 2x2=4 nodes! *\n"
      #debug "* * * * * * * * * * * * * * * * * * * * * * * * *\n"
    #end
#end // #macro DrawWireFrame()


/****M* WireSurf.inc/ImportDataRC()
* PURPOSE
*   Imports only data from input file.
*   Input data is organized as a matrix of floats separated by commas.
*   This macro is part of visualisation suite of matrices thus "RC" suffix (Row/Column).
* SYNOPSIS
ImportDataRC(string filename DataFileName)
* INPUTS
*   DataFileName - valid name of file with data - see example file for details
*   of its structure. Generally speaking first is Column axis title, Row Yaxis title,
*   number of columns, then min and max value assigned to the first and last item in row,
*   number of rows, then min and max value assigned to the first and last item in column,
*   eventually numeric data in form of matrix, ie. data measured, observed or
*   calculated over the grid
* SOURCE
/// structure  of the grid is interpreted as follows:
///  * -----> Col ---->
/// R|  r1c1 r1c2 r1c3 r1c4
/// o|  r2c1
/// w|  r3c1...
///  V  r4c1..
///  ie. rows are treated as x-data so they are drawn along REAL X axis (PovRAY '-Z')
****
/// #macro ImportDataRC()
*/
#macro ImportDataRC(DataFileName)  // matrix QNodes[] is created
    #fopen DataFile DataFileName read
    #read(DataFile, DescriptX, DescriptY)
    #read(DataFile, Nc, c1, c2)
    #read(DataFile, Nr, r1, r2)
    #if ((Nc>1)&(Nr>1))

        #declare QNodes = array[Nr][Nc];  // 3D vector: <x, value in node, z>
        #declare deltaC = (c2-c1)/(Nc-1); // along  X axis (povray)
        #declare deltaR = (r2-r1)/(Nr-1); // along -Z axis (povray)
        #declare QSurfSide=sqrt(deltaC*deltaR);

        #declare QNodes[0][0]=<0,0,0>;          // explicit typing array as float

        #declare MinY=0.0;
        #declare MaxY=0.0;

        #declare MinX=c1;
        #declare MaxX=c2;

        #declare BaseZL=r2;
        #declare NRow=1;

        #local _Yval=0.0;
        #declare CountR=0;
        #while(CountR<Nr)
          #declare CountC=0;
          #while(CountC<Nc)
               #read(DataFile, _Yval)
               #declare QNodes[CountR][CountC] = <r1+CountC*deltaC, _Yval, -c1-CountR*deltaR >;

               #if (_Yval<MinY)
                 #declare MinY=_Yval;
               #end

               #if (_Yval>MaxY)
                 #declare MaxY=_Yval;
               #end
             #declare CountC=CountC+1;
          #end
          #declare CountR=CountR+1;
        #end

        #fclose DataFile

        #debug concat("MaxY= ",str(MaxY,10,7), "\n********\n\n")

        #declare DscMaxY=MaxY;
        #declare MaxY=YScaleFactor*MaxY;
        #declare SceneMaxY=MaxY;
        #declare SceneMinX=c1;
        #declare SceneMaxX=c2;
        #declare SceneMinZ=-r1;
        #declare SceneMaxZ=-r2;
        #declare Ny=Nr;
        #declare Nx=Nc;
    #else
      #debug "* * * * * * * * * * * * * * * * * * * * * * * *\n"
      #debug "* To few nodes. You need at least 2x2=4 nodes! *\n"
      #debug "* * * * * * * * * * * * * * * * * * * * * * * *\n"
    #end

#end // ImportData()

/****M* WireSurf.inc/ImportDataXY()
* PURPOSE
*   Imports representation od data from input file.
*   Input data is organized as a matrix of floats separated by commas.
*   This macro is intendend for visualisation of gridded data thus "YX" suffix.
* SYNOPSIS
ImportDataXY(string filename DataFileName)
* INPUTS
*   DataFileName - valid name of file with data - see example file for details
*   of its structure. Generally speaking first is Xaxis title, Yaxis title,
*   number of columns, then min and max value assigned to the items in first and last X column respectively (along POVRay X axis),
*   number of rows, then min and max value assigned to the items in first and last row respectively (along POVRay -Z axis),
*   eventually numeric data in form of matrix, ie. data measured, observed or
*   calculated over the real XY grid
* SOURCE
/// structure  of the grid is interpreted as follows:
///  * -----> X real axis ---->
///  |  x1y1 x2y1 x3y1 x4y1
///  |  x1y2
/// Y|  x1y3...
///  V  X1y4..
///  ie. rows are treated as x-data so they are drawn
///  along REAL X  axis /-Z in PovRAY coords/ ,
****
/// #macro ImportDataXY(DataFileName)
*/
#macro ImportDataXY(DataFileName)  // matrix QNodes[] is created
    #fopen DataFile DataFileName read
    #read(DataFile, DescriptX, DescriptY)
    #read(DataFile, Nx, x1, x2)
    #read(DataFile, Ny, y1, y2)
    #if ((Nx>1)&(Ny>1))

        #declare QNodes=array[Ny][Nx];          // 3D vector: <x, value in node, z>
        #declare deltaX=(x2-x1)/(Nx-1);
        #declare deltaY=(y2-y1)/(Ny-1);
        #declare QSurfSide=sqrt(deltaX*deltaY);

        #declare QNodes[0][0]=<0,0,0>;          // explicit typing array as float

        #declare MinY=0.0;
        #declare MaxY=0.0;

        #declare MaxX=x2;

        #declare BaseZL=y2;
        #declare NRow=1;

        #local _Yval=0.0;
        #declare CountY=0;
        #while(CountY<Ny)
          #declare CountX=0;
          #while(CountX<Nx)
               #read(DataFile, _Yval)
               #declare QNodes[CountY][CountX] = <y1+CountY*deltaY, _Yval, -x1-CountX*deltaX >;

               #if (_Yval<MinY)
                 #declare MinY=_Yval;
               #end

               #if (_Yval>MaxY)
                 #declare MaxY=_Yval;
               #end
             #declare CountX=CountX+1;
          #end
          #declare CountY=CountY+1;
        #end

        #fclose DataFile

        #debug concat("MaxY= ",str(MaxY,10,7), "\n********\n\n")

        #declare DscMaxY=MaxY;
        #declare MaxY=YScaleFactor*MaxY;
        #declare SceneMaxY=MaxY;
        #declare SceneMinX=x1;
        #declare SceneMaxX=x2;
        #declare SceneMinZ=-y1;
        #declare SceneMaxZ=-y2;
    #else
      #debug "* * * * * * * * * * * * * * * * * * * * * * * *\n"
      #debug "* To few nodes. You need at least 2x2=4 nodes! *\n"
      #debug "* * * * * * * * * * * * * * * * * * * * * * * *\n"
    #end

#end // ImportData()

/****M* WireSurf.inc/DrawNodePoints()
* PURPOSE
*  Draws grid data points as ellipsoids ie. spheres scaled along vertical axis
*  by _scl factor. If _scl equals 1 one obtains pure sphere, however it can be lower or bigger than 1
*  This macro is designed to visualize uncertainty at given grid point
* SYNOPSIS
DrawNodePoints(float _scl)
* INPUTS
*  _scl - scaling factor along vertical axis, not perpendicularly to the surface!
*  this is intended for representation of uncertainty of all the data (SD? SEM?)
****/
#macro DrawNodePoints(_scl) //= pure geometry ! use object{....  texture } construct
    #for (iy, 0,Ny-1)
       #for (ix, 0, Nx-1)
           sphere{ 0 NodeRadiiCoeff*QSurfSide
                   scale <1, _scl ,1>
                   translate <QNodes[iy][ix].x, YScaleFactor*QNodes[iy][ix].y, QNodes[iy][ix].z >}
       #end
    #end
#end

/****M* WireSurf.inc/DrawLinearWires()
* PURPOSE
*  Draws surface representation in the form of grid of LINEAR splines, ie. straight cuts of line
* SYNOPSIS
DrawLinearWires(float _wireR, boolean _Sphrs)
* INPUTS
*  _wireR  - float   = radii of the wire
*  _Sphrs  - boolean = draw spheres in place of nodes? yes/no
* SEE ALSO
*  ImportDataRC()
*  ImportDataXY()
*  DrawNodePoints()
*  DrawCubicWires(),
*  DrawBSplineWires()
****/
#macro DrawLinearWires(_wireR, _Sphrs)
// was:    #local WireRadiiCoeff=_wireR*QSurfSide;
    #local _WRadii = (_wireR>Tiny?_wireR:Tiny)*WireRadiiCoeff*QSurfSide;  // adjustment of bar radii
    #local _NRadii = 1.41*_WRadii;                             
//#debug concat("QSurf=", str(QSurfSide,10,7), "; NodeR=", str(_NRadii,10,7), "; Wire radii=", str(_WRadii,10,7), "\n")
    //======O-X-> X_r axis (real)
    #for (iy, 0, Ny-1)
        sphere_sweep{//
             linear_spline Nx
                #for (ix,0,Nx-1)
                    ,<1,YScaleFactor,1>*(QNodes[iy][ix]), _WRadii
                #end
        }
    #end

    // =====O-Y->  Y_r axis  (real)
    #for (ix, 0, Nx-1)
        sphere_sweep{//
             linear_spline Ny
                #for (iy,0,Ny-1)
                    ,<1,YScaleFactor,1>*(QNodes[iy][ix]), _WRadii
                #end
        }
    #end
    #if (_Sphrs)
        union{ DrawNodePoints(1)}
    #end

#end // #macro DrawLinearWires()


/****M* WireSurf.inc/DrawCubicWires()
* PURPOSE
*  Draws surface representation in the form of grid of LINEAR splines, ie. straight cuts of line
* SYNOPSIS
DrawCubicWires(float _wireR, boolean _Sphrs)
* INPUTS
*  _wireR - float   = radii of the wire
*  _Sphrs - boolean = draw spheres in place of nodes? yes/no
* SEE ALSO
*  ImportDataRC()
*  ImportDataXY()
*  DrawNodePoints()
*  DrawLinearWires()
*  DrawBSplineWires()
****/
#macro DrawCubicWires(_wireR, _Sphrs)
// was:    #local WireRadiiCoeff=_wireR*QSurfSide;
    #local _WRadii = (_wireR>Tiny?_wireR:Tiny)*WireRadiiCoeff*QSurfSide;  // adjustment of bar radii
    #local _NRadii = 1.41*_WRadii;                             
//#debug concat("QSurf=", str(QSurfSide,10,7), "; NodeR=", str(_NRadii,10,7), "; Wire radii=", str(_WRadii,10,7), "\n")
    //======O-X-> X_r axis (real)
    #for (iy, 0, Ny-1)
        #declare Start = 2*QNodes[iy][0] - QNodes[iy][1];
        #declare End   = 2*QNodes[iy][Nx-1]-QNodes[iy][Nx-2];

        #ifdef(Debug)
            #if(Debug!=0)
                sphere{<1,YScaleFactor,1>*Start, .9*NodeRadiiCoeff texture{pigment{color rgb<1.0, .3,  .3>} finish{phong .7 reflection metallic}}}
                sphere{<1,YScaleFactor,1>*End,   .9*NodeRadiiCoeff texture{pigment{color rgb< .3, .3, 1.0>} finish{phong .7 reflection metallic}}}
            #end
        #end

        sphere_sweep{//
             cubic_spline Nx+2
             <1,YScaleFactor,1>*Start, _WRadii
                #for (ix,0,Nx-1)
                    ,<1,YScaleFactor,1>*(QNodes[iy][ix]), _WRadii
                #end
           , <1,YScaleFactor,1>*End, _WRadii
        }
    #end

    // =====O-Y->  Y_r axis  (real)
    #for (ix, 0, Nx-1)
        #declare Start = 2*QNodes[0][ix] - QNodes[1][ix];
        #declare End   = 2*QNodes[Ny-1][ix]-QNodes[Ny-2][ix];

        #ifdef(Debug)
            #if(Debug!=0)
               sphere{<1,YScaleFactor,1>*Start, .9*NodeRadiiCoeff texture{pigment{color rgb<1.0, .3,  .3>} finish{phong .7 reflection metallic}}}
               sphere{<1,YScaleFactor,1>*End,   .9*NodeRadiiCoeff texture{pigment{color rgb< .3, .3, 1.0>} finish{phong .7 reflection metallic}}}
            #end
        #end
        sphere_sweep{//
             cubic_spline Ny+2
             <1,YScaleFactor,1>*Start, _WRadii
                #for (iy,0,Ny-1)
                    ,<1,YScaleFactor,1>*(QNodes[iy][ix]), _WRadii
                #end
           , <1,YScaleFactor,1>*End, _WRadii
        }
    #end
    #if (_Sphrs)
        union{ DrawNodePoints(1)}
    #end

#end // #macro

/****M* WireSurf.inc/DrawBSplineWires()
* PURPOSE
*  Draws surface representation in the form of grid of Bezier splines, full implementation
* SYNOPSIS
DrawBSplineWires2B(float _Stfns, float _wireR, boolean _Sphrs)
* INPUTS                        
*  _Stfns - float > 0 = "straightness" of the spline; the bigger the more spline resembles straight lines;
*                     = length of control point handles = 1/_Stfns [p.u.], values <= 0 ignored 
*  _wireR - float > 0 = radii of the wire [p.u.], values <= 0 ignored, usually equals 1.0
*  _Sphrs - boolean   = draw spheres in place of nodes? yes/no
* SEE ALSO
*  ImportDataRC()
*  ImportDataXY()
*  DrawNodePoints()
*  DrawLinearWires()
*  DrawCubicWires()
****/
#macro DrawBSplineWires(_StfnsX, _StfnsY, _wireR, _Sphrs)
    #local _WRadii = (_wireR>Tiny?_wireR:Tiny)*WireRadiiCoeff*QSurfSide;  // adjustment of bar radii
    #local _NRadii = 1.41*_WRadii;    
//#debug concat("QSurf=", str(QSurfSide,10,7), "; NodeR=", str(_NRadii,10,7), "; Wire radii=", str(_WRadii,10,7), "\n")                             
    #if (_StfnsX > 1e-7) #local _LHandleX = 1/_StfnsX; #else #local _LHandleX = 1e7; #end
    #if (_StfnsY > 1e-7) #local _LHandleY = 1/_StfnsY; #else #local _LHandleY = 1e7; #end
    //======O-X-> X_r axis (real) by cuts
    #for (iy, 0, Ny-1)
        #declare Start = QNodes[iy][   0] + _LHandleX*(QNodes[iy][   0] - QNodes[iy][   1])/vlength(QNodes[iy][   0] - QNodes[iy][   1]);
        #declare End   = QNodes[iy][Nx-1] + _LHandleX*(QNodes[iy][Nx-1] - QNodes[iy][Nx-2])/vlength(QNodes[iy][Nx-1] - QNodes[iy][Nx-2]);

        #ifdef(Debug)
            #if(Debug!=0)
              sphere{<1,YScaleFactor,1>*Start, 1.5*_NRadii texture{pigment{color rgb<.0, 1,   1>} finish{phong .7 reflection metallic}}}
              sphere{<1,YScaleFactor,1>*End,   1.5*_NRadii texture{pigment{color rgb< 1,  1,  0>} finish{phong .7 reflection metallic}}}
            #end
        #end

        sphere_sweep{//
              b_spline 4*Nx-2
              <1,YScaleFactor,1>*Start, _WRadii
             ,<1,YScaleFactor,1>*(QNodes[iy][  0]), _WRadii
             ,<1,YScaleFactor,1>*(QNodes[iy][  0]+_LHandleX*(QNodes[iy][  1]-QNodes[iy][  0])/vlength(QNodes[iy][  1]-QNodes[iy][  0])), _WRadii
             ,<1,YScaleFactor,1>*(QNodes[iy][  1]-_LHandleX*(QNodes[iy][  2]-QNodes[iy][  0])/vlength(QNodes[iy][  2]-QNodes[iy][  0])), _WRadii
             ,<1,YScaleFactor,1>*(QNodes[iy][  1]), _WRadii

             #for (ix,1,Nx-3)
                 ,<1,YScaleFactor,1>*(QNodes[iy][  ix]), _WRadii
                 ,<1,YScaleFactor,1>*(QNodes[iy][  ix]+_LHandleX*(QNodes[iy][ix+1]-QNodes[iy][ix-1])/vlength(QNodes[iy][ix+1]-QNodes[iy][ix-1])), _WRadii

                 ,<1,YScaleFactor,1>*(QNodes[iy][ix+1]-_LHandleX*(QNodes[iy][ix+2]-QNodes[iy][ix])/vlength(QNodes[iy][ix+2]-QNodes[iy][ix])), _WRadii
                 ,<1,YScaleFactor,1>*(QNodes[iy][ix+1]), _WRadii
             #end

             ,<1,YScaleFactor,1>*(QNodes[iy][Nx-2]), _WRadii
             ,<1,YScaleFactor,1>*(QNodes[iy][Nx-2]+_LHandleX*(QNodes[iy][Nx-1]-QNodes[iy][Nx-3])/vlength(QNodes[iy][Nx-1]-QNodes[iy][ix-3])), _WRadii

             ,<1,YScaleFactor,1>*(QNodes[iy][Nx-1]-_LHandleX*(QNodes[iy][Nx-1]-QNodes[iy][Nx-2])/vlength(QNodes[iy][Nx-1]-QNodes[iy][Nx-2])), _WRadii
             ,<1,YScaleFactor,1>*(QNodes[iy][Nx-1]), _WRadii

             ,<1,YScaleFactor,1>*End, _WRadii
        } // sphere_sweep/X
  #end

//=============================
// =====O-Y->  Y_r axis  (real)
    #for (ix, 0, Nx-1)
        #declare Start = QNodes[   0][ix] + _LHandleY*(QNodes[   0][ix] - QNodes[   1][ix])/vlength(QNodes[   0][ix] - QNodes[   1][ix]);
        #declare End   = QNodes[Ny-1][ix] + _LHandleY*(QNodes[Ny-1][ix] - QNodes[Ny-2][ix])/vlength(QNodes[Ny-1][ix] - QNodes[Ny-2][ix]);

         #ifdef(Debug)
             #if(Debug!=0)
               sphere{<1,YScaleFactor,1>*Start, 1.5*_NRadii texture{pigment{color rgb<.0, 1,   1>} finish{phong .7 reflection metallic}}}
               sphere{<1,YScaleFactor,1>*End,   1.5*_NRadii texture{pigment{color rgb< 1,  1,  0>} finish{phong .7 reflection metallic}}}
             #end
         #end

         sphere_sweep{//
              b_spline 4*Ny-2
              <1,YScaleFactor,1>*Start, _WRadii
             ,<1,YScaleFactor,1>*(QNodes[0][ix]), _WRadii
             ,<1,YScaleFactor,1>*(QNodes[0][ix]+_LHandleY*(QNodes[1][ix]-QNodes[0][ix])/vlength(QNodes[1][ix]-QNodes[0][ix])), _WRadii
             ,<1,YScaleFactor,1>*(QNodes[1][ix]-_LHandleY*(QNodes[2][ix]-QNodes[0][ix])/vlength(QNodes[2][ix]-QNodes[0][ix])), _WRadii
             ,<1,YScaleFactor,1>*(QNodes[1][ix]), _WRadii

             #for (iy,1,Ny-3)
                 ,<1,YScaleFactor,1>*(QNodes[iy][  ix]), _WRadii
                 ,<1,YScaleFactor,1>*(QNodes[iy][  ix]+_LHandleY*(QNodes[iy+1][ix]-QNodes[iy-1][ix])/vlength(QNodes[iy+1][ix]-QNodes[iy-1][ix])), _WRadii

                 ,<1,YScaleFactor,1>*(QNodes[iy+1][ix]-_LHandleY*(QNodes[iy+2][ix]-QNodes[iy][ix])/vlength(QNodes[iy+2][ix]-QNodes[iy][ix])), _WRadii
                 ,<1,YScaleFactor,1>*(QNodes[iy+1][ix]), _WRadii
             #end

             ,<1,YScaleFactor,1>*(QNodes[Ny-2][ix]), _WRadii
             ,<1,YScaleFactor,1>*(QNodes[Ny-2][ix]+_LHandleY*(QNodes[Ny-1][ix]-QNodes[Ny-3][ix])/vlength(QNodes[Ny-1][ix]-QNodes[Ny-3][ix])), _WRadii

             ,<1,YScaleFactor,1>*(QNodes[Ny-1][ix]-_LHandleY*(QNodes[Ny-1][ix]-QNodes[Ny-2][ix])/vlength(QNodes[Ny-1][ix]-QNodes[Ny-2][ix])), _WRadii
             ,<1,YScaleFactor,1>*(QNodes[Ny-1][ix]), _WRadii

             ,<1,YScaleFactor,1>*End, _WRadii
         }// sphere_sweep/Y
  #end

    #if (_Sphrs)
        union{ DrawNodePoints(1)}
    #end
#end // #macro

//============
//============
#end // #endif
