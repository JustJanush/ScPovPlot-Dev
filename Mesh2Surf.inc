/****h* ScPovPlot3D/Mesh2Surf.inc
* PURPOSE
*   Macros for representation of 3D surfaces as mesh of triangles,
*   for this goal object mesh2{} is employed
*     image:./imgs/ColorMapS.png
*     |html <hr width=50% align="left">
*     image:./imgs/Num2SurfS.jpg
*   Fig.[Mesh2Surf] 3D surface modelled by mesh2 object
*   |html <hr width=50% align="left">
*
* VERSION
*  v. 4.0.0, tested on PovRay 3.7
*
* AUTHOR
*  Janusz Opi³a Ph.D.
*   jmo@agh.edu.pl, janusz.opila@gmail.com    
*   Homepage: http://scpovplot3d.sourceforge.net
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

// TODO: Interval computations - implementation of "fuzzy points"
#ifndef(Mesh2Surf_INC)
#declare Mesh2Surf_INC = "2021-06-10"; // last modification date
// common includes & definitions
#version 3.7;
#declare _FileName   = input_file_name;
#declare _FileVerMaj = "4.0.0";
#declare _FileVerMin = Mesh2Surf_INC;

#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")
     
#ifndef (Debug) // Just 4 safety
   #declare Debug=0;
#end     
     
#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"
#include "glass.inc"
#include "math.inc"

/*********************/
#include "CommonDefs.inc"   // This module always gets first
#include "Cameras.inc"      // Useful cameras
#include "scFinish"
#include "TextExt.inc"

#declare YScaleFactor = 1.0; // exaggerates vertical dimension: default
#macro SetYScaleFactor(YSF)
    #declare YScaleFactor = YSF;
#end
#macro AdjustMaxY(Y) // Overrides automatically evaluated number
    #declare DscMaxY = Y;
    #declare MaxY= DscMaxY*YScaleFactor;
#end


// these variables get reasonable values after call to DrawSurface () macro
#declare MinX = 0;
//#declare BaseZL = 0;
#declare MaxX   = 0;
#declare deltaX = 0;
#declare deltaY = 0;

#declare tmpColour   = rgb <0,0,1>; // well, I like blue colour...
#declare tmpColour1  = rgbf<0.2, 0.3, 0.4, 0.1>;
#declare tmpColour2  = rgbf<1.5, 1.3, 1.2, 0.3>;
#declare tmpInterior = interior{inGlass};
#declare tmpTexture  = txBlue;
#declare tmpMaterial = mtGlass;          

#declare Filter  =     0;          
#declare Warstwy =   yes; 
#declare LevCol  = Black;    

// initial settings
#declare SurfTexture  = texture{ Silver_Metal };
#declare SurfFinish   = finish { scDullMirror };

#declare InterTexture = texture{ Silver_Metal };
#declare InterFinish  = finish { Dull };

#declare SurfTexture = // well, it is just texture definition
      texture{ 
         pigment{
           gradient y
           color_map	{
           	 [0.000 color NewMidnightBlue  filter Filter]
           	 [0.100 color Blue filter Filter]  
           	 [0.148 color Cyan filter Filter]  
           	 [0.245 color Green filter Filter]
             #if ( Warstwy )
              	 [0.245 color LevCol ]
                 [0.250 color LevCol ]        	
              	 [0.250 color Green filter Filter]  //  yellow lemon
             #end        	

             [0.400 color rgbf< 0.75, 1.0, 0.0, Filter>]     //  yellow lemon

             [0.495 color Yellow  filter Filter]
             #if ( Warstwy )
              	 [0.495 color LevCol ]
                 [0.500 color LevCol ]        	
              	 [0.500 color Yellow filter Filter]
             #end        	

            [0.545 color Gold  filter Filter]
            [0.575 color Orange  filter Filter]

            [0.745 color OrangeRed  filter Filter]
            #if ( Warstwy )        	
                 [0.745 color LevCol ]
              	 [0.750 color LevCol ]        	
               	 [0.750  color OrangeRed filter Filter]        
             #end         	
             [0.875 color Firebrick filter Filter]     //  very dark red  
             [0.990 color rgbf< 1.0, 0.0, 0.0, Filter>]
           } //color_map
         }   //pigment
       };           


/****f* Mesh2Surf.inc/Set_SurfTexture
* PURPOSE
*  moves 
* SYNOPSIS
*/
#macro Set_SurfTexture( _tt )
/*
 * INPUTS
 *  _tt - texture to be applied
 * SEE ALSO
 *  empty
 ****/
    #declare SurfTexture = _tt; 
#end

/****f* Mesh2Surf.inc/Set_SurfFinish, shiftXY
* PURPOSE
*  moves 
* SYNOPSIS
*/
#macro Set_SurfFinish( _tt )
/*
 * INPUTS
 *  _tt - texture to be applied
 * SEE ALSO
 *  empty
 ****/
    #declare SurfFinish =_tt;  
#end    
    
/****f* Mesh2Surf.inc/Set_InterTexture, shiftXY
* PURPOSE
*  moves 
* SYNOPSIS
*/
#macro Set_InterTexture( _tt )
/*
 * INPUTS
 *  _tt - texture to be applied
 * SEE ALSO
 *  empty
 ****/
    #declare InterTexture = _tt; 
#end

/****f* Mesh2Surf.inc/Set_InterFinish, shiftXY
* PURPOSE
*  moves 
* SYNOPSIS
*/
#macro Set_InterFinish( _tt )
/*
 * INPUTS
 *  _tt - texture to be applied
 * SEE ALSO
 *  empty
 ****/
    #declare InterFinish = _tt; 
#end


/****v* Mesh2Surf.inc/DeclareLevel
* PURPOSE
*   DeclareLevel determines what is defined:
*   * 0 = colour only
*   * 2 = also texture
*   * 4 = complete material
*   this value is valid until next "Set...()" declaration is met.
* SOURCE 
*/
#declare DeclareLevel = 0; // 2in1 - doc entry & initialization
/*****/            

/// initial default safe value
/****f* Mesh2Surf.inc/Set_TextFont, _TextFont
* PURPOSE
*  Defines font for textual elements of the chart
*    WARNING: Font must exist in the host operating system
*    default setting "Arial.ttf" should be safe on Windows, as well as on some GNU/Linux distros
* SYNOPSIS
*/
#declare _TextFont = "Arial.ttf"; // "arialuni.ttf" is not safe as this font not always exist in the system
#macro Set_TextFont (_TxtFnt)
/*
 * INPUTS
 *  _TxtFnt - string containing name of the file containing given font, ex. "Arial.ttf". Sets internal local variable _TextFont
 * SEE ALSO 
 *  Set_TFont ()
 ****/
    #declare _TextFont = _TxtFnt;
#end




#declare Photons=false;
#declare NodeRadiiCoeff=0.20;
#declare WireRadiiCoeff=NodeRadiiCoeff/2;

#declare shiftXY    = -1.4;
#declare LettrSize  =  0.8;
#declare LettrDepth =  0.2;
#declare LettrBase  = -0.7;
#declare DscrAngle  =  0.0;
#declare NumDscrLgh =5;
#declare NumDscrDig =3;

#declare NRow = 1;
#declare NCol = 1;

#declare ShowPoints=no;

#macro SetShowPoints()
    #declare ShowPoints=yes;
#end

#macro SetLettrSize(ls)
    #declare LettrSize = ls;
#end
#macro SetLettrDepth(ls)
    #declare LettrDepth= ls;
#end

#macro SetLettrBase(ls)
    #declare LettrBase =ls;
#end

#macro SetDscrAngle(ls)
    #declare DscrAngle=ls;
#end

#macro SetBgPicture(Pct)
  #declare BgPicture=Pct;
#end // macro SetBgPicture()

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

#macro SetTexture( FullTexture ) // follow it by SetInterior("InteriorDefinition")
    #declare tmpTexture = FullTexture ;
    #declare DeclareLevel = 2;
#end

#macro SetInterior( tInt ) // valid only together with texture defintions
    #declare tmpInterior = interior{tInt};
    #declare DeclareLevel = 2;
#end

#macro SetMaterial( FullMaterial )
    #declare tmpMaterial = FullMaterial;
    #declare DeclareLevel = 4;
#end

#macro SetNodeRadiiCoeff(Coeff)
  #declare NodeRadiiCoeff=Coeff;
  #declare WireRadiiCoeff=NodeRadiiCoeff/2;
#end

#macro SetWireRadiiCoeff(Coeff)
  #declare WireRadiiCoeff=Coeff;
#end

/****f* Mesh2Surf.inc/Set_ShiftXY, shiftXY
* PURPOSE
*  moves vertical description labels by that amount, can change it later on..
* SYNOPSIS
*/
#macro Set_ShiftXY(shft)
/*
 * INPUTS
 *  shft - float - distance in POVRay units along X_P axis
 * SEE ALSO
 *  empty
 ****/
    #declare shiftXY = shft;
#end

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

/**************************************/
#macro SetNumFormat(L, D)
    #declare NumDscrLgh=L;
    #declare NumDscrDig=D;
#end                   

#macro SetXDscOffset( _ff)
   #declare shiftXY = _ff;
#end
                          
#declare shiftZZ = 0.1;                          
#macro SetZDscOffset( _ff)
   #declare shiftZZ = _ff;
#end

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
         ttf _TextFont ,str( DscrVrtAxe[Cnt], NumDscrLgh, NumDscrDig), LettrDepth, 0 texture{_Texture}
         scale LettrSize
         rotate DscrAngle*y
         translate<shiftXY, YScaleFactor*(.97*DscrVrtAxe[Cnt]), MaxZ-shiftZZ >
      }
    #declare Cnt=Cnt+1;
    #end

  #end //#if
#end

#macro InitDscrColAxe(N)
    #declare DscrColAxe = array[N];       // description along X-axis (ie column axe)
    #declare NCol=N;
    #declare dX = ((N>1)?(MaxX-MinX)/(N-1):0);
#end

/** X>************************************/
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

//Xnn ->
#macro DrawColDscr( _Txtr, _LettrBase, _LettrSize, _LettrDepth) // Texture, Letter base
  #declare xPos = MinX+shiftXY;
  #declare Cnt  = 0;
  Set_TFont( _TextFont  )  //n
  SetLDpth( _LettrDepth )  // set depth of letters (z-axis size) by SetLDpth() macro
#debug concat("deltaX= ",str(deltaX,5,3), ";  deltaY= ",str(deltaY,5,3) "\n********\n\n")

  #while(Cnt<NCol)
    #declare TxtTmp = object { Text(DscrColAxe[Cnt])  scale _LettrSize };
    #local shft = (max_extent(TxtTmp).x+min_extent(TxtTmp).x)/2.0;
     object { TxtTmp
        texture{_Txtr}     //n
        
        translate -shft    // move to origin
        rotate DscrAngle*y //n
        translate  shft    // ... then transfer back                  
        translate<xPos, _LettrBase, MaxZ-shiftZZ>
     }

    #declare xPos=xPos+dX;
    #declare Cnt=Cnt+1;
  #end
#end


/** Z>************************************/
#macro InitDscrRowAxe(N)
    #declare DscrRowAxe = array[N];     // description along Z-axis (ie row axe)
    #declare NRow = N;     // description along Z-axis (ie row axe)
    #declare dZ = ((N>1)?(MaxZ-MinZ)/(N-1):0);    
#end

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

//Znn
/**************************************/
#macro DrawRowDscr( _Txtr, _LettrBase, _LettrSize, _LettrDepth) // Texture, Letter base
  #declare ZPos = 0.0;
  #declare XPos = MaxX+shiftXY;
  #declare Cnt=0;
#debug concat("deltaX= ",str(deltaX,5,3), ";  deltaY= ",str(deltaY,5,3) "\n********\n\n")
  Set_TFont( _TextFont  )  //n
  SetLDpth( _LettrDepth )  // set depth of letters (z-axis size) by SetLDpth() macro

  #while(Cnt<NRow)
    #declare TxtTmp = object { Text(DscrRowAxe[Cnt])  scale _LettrSize };
    #local shft = (max_extent(TxtTmp).x+min_extent(TxtTmp).x)/2.0;
     object { TxtTmp
        texture{_Txtr}     //n
        translate -shft    // move to origin
        rotate DscrAngle*y //n
        translate  shft    // ... then transfer back                  
        translate<XPos, _LettrBase, ZPos>
     }

    #declare ZPos=ZPos+dZ;
    #declare Cnt=Cnt+1;
  #end
  #declare NRow=1;
#end
/**************************************/

/****f* Mesh2Surf.inc/DrawSurface ()
* PURPOSE
*  Draws surface representation in the form of smooth mesh of triangles
* SYNOPSIS
DrawSurface(string filename DataFileName)
* INPUTS
*  DataFileName - valid name of file with data - see example file for details
*  of its structure. Generally speaking first is Xaxis title, Yaxis title,
*  number of columns, then min and max value assigned to the items in first and last X column respectively (along POVRay X axis),
*  umber of rows, then min and max value assigned to the items in first and last row respectively (along POVRay -Z axis),
*  eventually numeric data in form of matrix, ie. data measured, observed or
*  calculated over the real XY grid
* SEE ALSO
*  WireSurf.inc
*  BPatchSurf.inc
*  DrawSurfaceOnBlock ()
****/
#macro DrawSurface(DataFileName)
// At present MinX := x1 !!!
    #fopen DataFile DataFileName read
    #read(DataFile, DescriptX, DescriptY)
    #read(DataFile, Nx, x1, x2)
    #read(DataFile, Ny, y1, y2)
    #if ((Nx>1)&(Ny>1))

        #declare Nodes=array[Ny][Nx];          // float: height of nodes only

        #declare QNodes=array[Nx*Ny+(Nx-1)*(Ny-1)];      // float vectors: full mesh2 "vertices"
        #declare QNodes[0]=<0,0,0>;                      // explicit typing QNode as vectors

        #declare Pointers=array[Ny][Nx];                 // integer pointers to QNodes array/temporary
        #declare Pointers[0][0]=0;                       // QNodes[Pointers[i][j]]=> node Nx,Ny
                                                         // QNodes[nx*Y+X]=>Nodes[X,Y]

        #declare Quads=array[Ny-1][Nx-1][5];             // quintet of integer pointers to corners and center of rectangles

//        #declare QNodesC=array[(Nx-1)*(Ny-1)];         // float: height of centers of rectangles
//        #declare Triangs=array[4*(Nx-1)*(Ny-1)][3]; // triplets of integer pointers to vertices of triangles

        #declare deltaX=(x2-x1)/(Nx-1);
        #declare deltaY=(y2-y1)/(Ny-1);
//#debug concat("deltaX= ",str(deltaX,5,3), ";  deltaY= ",str(deltaY,5,3) "\n********\n\n")
        #declare CountY=0;
        #declare Nodes[0][0]=0.0;                       // explicit typing array as float

        #declare MinY = 0.0;
        #declare MaxY = 0.0;     /// this is UGLY!

        #declare MinX = x1;
        #declare MaxX = x2;

        #declare MinZ =  -y1; // remember of reversed POVRay coordinates
        #declare MaxZ =  -y2; // remember of reversed POVRay coordinates

        #declare NRow=1;

        #declare QSurfSide=sqrt(deltaX*deltaY);         // try to estimate nice values for
        #declare NodeRadii=NodeRadiiCoeff*QSurfSide;    // NodeRadiiCoeff &
        #declare WireRadii=WireRadiiCoeff*QSurfSide;    // WireRadiiCoeff

        #debug concat("Just reading ", DataFileName,"\n")
        #local JustRead=0.0;
        #declare CountY=0;
        #while(CountY<Ny)
          #declare CountX=0;
          #while(CountX<Nx)
        //#debug concat("\n\n********\nCountX= ",str(CountX,5,0), "\n")
        //#debug concat("CountY= ",str(CountY,5,0), "\n********\n\n")
        //#debug DataFileName
               #read(DataFile, JustRead) 
               #if (JustRead < MinY)
                 #declare MinY = JustRead;
               #end

               #if (JustRead > MaxY)
                 #declare MaxY = JustRead;
               #end
             #declare Nodes[CountY][CountX]=JustRead;
             #declare CountX=CountX+1;
          #end
          #declare CountY=CountY+1;
        #end
        #fclose DataFile
        
// here search for MinY, MaxY
//
//
// end        

// matrix data just read in
// Lets analyse them....

//        #debug concat("SceneMaxY= ",str(SceneMaxY,10,7), "\n********\n\n")
        #declare DscMaxY=MaxY;
        #declare MinY=YScaleFactor*MinY;
        #declare MaxY=YScaleFactor*MaxY; // added 2012-11-10
        #declare SceneMinY=MinY;
        #declare SceneMaxY=MaxY;
        #declare SceneMinX=x1;
        #declare SceneMaxX=x2;
        #declare SceneMinZ=-y1;
        #declare SceneMaxZ=-y2;

// Now transform Nodes(x,y)=>QNodes(i)<>
        #declare CountY=0;
        #declare QCount=0; // QNodes index
        #while(CountY<Ny)
          #declare CountX=0;
          #while(CountX<Nx)
             #declare QNodes[CountY*Nx+CountX]=<x1+CountX*deltaX, Nodes[CountY][CountX], y1+CountY*deltaY>; // "real world" coordinates
             #declare Pointers[CountY][CountX]=QCount;

             #declare CountX=CountX+1;
             #declare QCount=QCount+1;
          #end
          #declare CountY=CountY+1;
        #end

//Central points

        #declare QuaN=0; // Quads counter
        #declare CountY=0;
        #while(CountY<Ny-1)
          #declare CountX=0;
          #while(CountX<Nx-1)
            #declare IndexA=Pointers[CountY][CountX];           //  A****B
            #declare IndexB=Pointers[CountY][CountX+1];         //  *    *
            #declare IndexC=Pointers[CountY+1][CountX+1];       //  *    *
            #declare IndexD=Pointers[CountY+1][CountX];         //  D****C

            #declare QNodes[QCount]=(<QNodes[IndexA].x, QNodes[IndexA].y, QNodes[IndexA].z>+
                                     <QNodes[IndexB].x, QNodes[IndexB].y, QNodes[IndexB].z>+
                                     <QNodes[IndexC].x, QNodes[IndexC].y, QNodes[IndexC].z>+
                                     <QNodes[IndexD].x, QNodes[IndexD].y, QNodes[IndexD].z>)
                                     /4;
  //      #debug concat("CountY= ",str(CountY,5,0), "   CountX= ",str(CountX,5,0),  "\n********\n\n")
            #declare Quads[CountY][CountX][0]=IndexA; //
            #declare Quads[CountY][CountX][1]=IndexB;
            #declare Quads[CountY][CountX][2]=IndexC;
            #declare Quads[CountY][CountX][3]=IndexD;
            #declare Quads[CountY][CountX][4]=QCount;

            #declare QuaN=QuaN+1; //next
            #declare CountX=CountX+1;
            #declare QCount=QCount+1;
          #end
          #declare CountY=CountY+1;
        #end


        #if (ShowPoints)       // need to parametrize texture as a separate object
            #declare Count=0;
            #while(Count<Ny*Nx+(Nx-1)*(Ny-1))
                   sphere {
                           #declare YYY=YScaleFactor*QNodes[Count].y+Tiny;
                          <QNodes[Count].x, YYY, -QNodes[Count].z>, NodeRadii

                          material {
                          texture {
                            pigment {rgbt <QNodes[Count].y/DscMaxY,.0,1-QNodes[Count].y/DscMaxY,  0.3>}
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
                            fade_color <QNodes[Count].y/DscMaxY,.5,1-QNodes[Count].y/DscMaxY>
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
               #declare Count=Count+1;
            #end
        #end

        mesh2{
    // vertices are stored in QNodes[..]
            vertex_vectors{
                #declare Count=0;
                Ny*Nx+(Nx-1)*(Ny-1)
                #while(Count<Ny*Nx+(Nx-1)*(Ny-1))
                    #declare YYY=YScaleFactor*QNodes[Count].y/MaxY+Tiny;
                    ,<QNodes[Count].x, YYY, -QNodes[Count].z>
                    #declare Count=Count+1;
                #end
            }

            face_indices{
                4*(Nx-1)*(Ny-1),
                #declare CountY=0;
                #while(CountY<Ny-1)
                  #declare CountX=0;
                  #while(CountX<Nx-1)
                      <Quads[CountY][CountX][4], Quads[CountY][CountX][1], Quads[CountY][CountX][0]>
                      <Quads[CountY][CountX][4], Quads[CountY][CountX][2], Quads[CountY][CountX][1]>
                      <Quads[CountY][CountX][4], Quads[CountY][CountX][3], Quads[CountY][CountX][2]>
                      <Quads[CountY][CountX][4], Quads[CountY][CountX][0], Quads[CountY][CountX][3]>
                    #declare CountX=CountX+1;
                  #end
                  #declare CountY=CountY+1;
                #end 
           }

        texture{ SurfTexture finish { SurfFinish } }
        interior_texture{ InterTexture finish { InterFinish }}                                   
        scale <1,MaxY,1>
        }     /// mesh2
//        #declare shiftXY = -0.05*(SceneMinX+SceneMaxX)/2; // Attempts to automatically determinate initial X description position
   #end   // if (Nx, Ny)

#end    // macro DrawSurface(DataFileName)...

/**************************************/

// TODO: just plain copy of DrrawSurface - to be reworked..
/****f* Mesh2Surf.inc/DrawSurfaceOnBlock ()
* PURPOSE
*  Draws surface representation in the form of smooth mesh of triangles
* SYNOPSIS
DrawSurfaceOnBlock(string filename DataFileName)
* INPUTS
*  DataFileName - valid name of file with data - see example file for details
*  of its structure. Generally speaking first is Xaxis title, Yaxis title,
*  number of columns, then min and max value assigned to the items in first and last X column respectively (along POVRay X axis),
*  umber of rows, then min and max value assigned to the items in first and last row respectively (along POVRay -Z axis),
*  eventually numeric data in form of matrix, ie. data measured, observed or
*  calculated over the real XY grid
* SEE ALSO
*  WireSurf.inc
*  BPatchSurf.inc
*  DrawSurface ()
****/
#macro DrawSurfaceOnBlock(DataFileName) // may someone propose better name?!
// Now MinX := x1 !!!
    #fopen DataFile DataFileName read
    #read(DataFile, DescriptX, DescriptY)
    #read(DataFile, Nx, x1, x2)
    #read(DataFile, Ny, y1, y2)
    #if ((Nx>1)&(Ny>1))

        #declare Nodes=array[Ny][Nx];          // float: height of nodes only

        #declare QNodes=array[Nx*Ny+(Nx-1)*(Ny-1)];      // float vectors: full mesh2 "vertices"
        #declare QNodes[0]=<0,0,0>;                      // explicit typing QNode as vectors

        #declare Pointers=array[Ny][Nx];                 // integer pointers to QNodes array/temporary
        #declare Pointers[0][0]=0;                       // QNodes[Pointers[i][j]]=> node Nx,Ny
                                                         // QNodes[nx*Y+X]=>Nodes[X,Y]

        #declare Quads=array[Nx-1][Ny-1][5];             // quintet of integer pointers to corners and center of rectangles

//        #declare QNodesC=array[(Nx-1)*(Ny-1)];         // float: height of centers of rectangles
//        #declare Triangs=array[4*(Nx-1)*(Ny-1)][3]; // triplets of integer pointers to vertices of triangles

        #declare deltaX=(x2-x1)/(Nx-1);
        #declare deltaY=(y2-y1)/(Ny-1);

        #declare CountY=0;
        #declare Nodes[0][0]=0.0;                       // explicit typing array as float

        #declare MinY=0.0;
        #declare MaxY=0.0;

        #declare MinX = x1;
        #declare MaxX = x2;

        #declare MinZ = -y1; // remember of reversed POVRay coordinates
        #declare MaxZ = -y2; // remember of reversed POVRay coordinates

//        #declare BaseZL=y2;
        #declare NRow=1;

//        #declare QSurfSide=sqrt(deltaX*deltaY);         // try to estimate nice values for
//        #declare NodeRadii=NodeRadiiCoeff*QSurfSide;    // NodeRadiiCoeff &
//        #declare WireRadii=WireRadiiCoeff*QSurfSide;    // WireRadiiCoeff

        #while(CountY<Ny)
          #declare CountX=0;
          #while(CountX<Nx)
        //#debug concat("\n\n********\nCountX= ",str(CountX,5,0), "\n")
        //#debug concat("CountY= ",str(CountY,5,0), "\n********\n\n")
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
        #fclose MyFile

// matrix data just read in
// Lets analyse them....

        #debug concat("SceneMaxY= ",str(SceneMaxY,10,7), "\n********\n\n")

        #declare DscMaxY=MaxY;
        #declare MaxY=YScaleFactor*MaxY;
        #declare SceneMinY=MinY;
        #declare SceneMaxY=MaxY;
        #declare SceneMinX=x1;
        #declare SceneMaxX=x2;
        #declare SceneMinZ=-y1;
        #declare SceneMaxZ=-y2;

// Now transform Nodes(x,y)=>QNodes(i)<>
        #declare CountY=0;
        #declare QCount=0; // QNodes index
        #while(CountY<Ny)
          #declare CountX=0;
          #while(CountX<Nx)
             #declare QNodes[CountY*Nx+CountX]=<x1+CountX*deltaX, Nodes[CountY][CountX], y1+CountY*deltaY>; // "real world" coordinates
             #declare Pointers[CountY][CountX]=QCount;

             #declare CountX=CountX+1;
             #declare QCount=QCount+1;
          #end
          #declare CountY=CountY+1;
        #end

//Central points

        #declare QuaN=0; // Quads counter
        #declare CountY=0;
        #while(CountY<Ny-1)
          #declare CountX=0;
          #while(CountX<Nx-1)
            #declare IndexA=Pointers[CountY][CountX];           //  A****B
            #declare IndexB=Pointers[CountY][CountX+1];         //  *    *
            #declare IndexC=Pointers[CountY+1][CountX+1];       //  *    *
            #declare IndexD=Pointers[CountY+1][CountX];         //  D****C

            #declare QNodes[QCount]=(<QNodes[IndexA].x, QNodes[IndexA].y, QNodes[IndexA].z>+
                                     <QNodes[IndexB].x, QNodes[IndexB].y, QNodes[IndexB].z>+
                                     <QNodes[IndexC].x, QNodes[IndexC].y, QNodes[IndexC].z>+
                                     <QNodes[IndexD].x, QNodes[IndexD].y, QNodes[IndexD].z>)
                                     /4;

            #declare Quads[CountY][CountX][0]=IndexA; //
            #declare Quads[CountY][CountX][1]=IndexB;
            #declare Quads[CountY][CountX][2]=IndexC;
            #declare Quads[CountY][CountX][3]=IndexD;
            #declare Quads[CountY][CountX][4]=QCount;

            #declare QuaN=QuaN+1; //next
            #declare CountX=CountX+1;
            #declare QCount=QCount+1;
          #end
          #declare CountY=CountY+1;
        #end

        #if (ShowPoints)
            #declare Count=0;
            #while(Count<Ny*Nx+(Nx-1)*(Ny-1))
                   sphere {
                           #declare YYY=YScaleFactor*QNodes[Count].y;
                          <QNodes[Count].x, YYY, -QNodes[Count].z>,

                          material {
                          texture {
                            pigment {rgbt <QNodes[Count].y/DscMaxY,.0,1-QNodes[Count].y/DscMaxY,  0.3>}
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
                            fade_color <QNodes[Count].y/DscMaxY,.5,1-QNodes[Count].y/DscMaxY>
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
               #declare Count=Count+1;
            #end
        #end

        mesh2{
    // vertices are stored in QNodes[..]
            vertex_vectors{
                #declare Count=0;
                Ny*Nx+(Nx-1)*(Ny-1)
                #while(Count<Ny*Nx+(Nx-1)*(Ny-1))
                    #declare YYY=YScaleFactor*QNodes[Count].y/MaxY+Tiny;
                    ,<QNodes[Count].x, YYY, -QNodes[Count].z>
                    #declare Count=Count+1;
                #end
            }

            face_indices{
                4*(Nx-1)*(Ny-1),
                #declare CountY=0;
                #while(CountY<Ny-1)
                  #declare CountX=0;
                  #while(CountX<Nx-1)
                      <Quads[CountY][CountX][4], Quads[CountY][CountX][1], Quads[CountY][CountX][0]>
                      <Quads[CountY][CountX][4], Quads[CountY][CountX][2], Quads[CountY][CountX][1]>
                      <Quads[CountY][CountX][4], Quads[CountY][CountX][3], Quads[CountY][CountX][2]>
                      <Quads[CountY][CountX][4], Quads[CountY][CountX][0], Quads[CountY][CountX][3]>
                    #declare CountX=CountX+1;
                  #end
                  #declare CountY=CountY+1;
                #end
            }

            texture{ SurfTexture finish {SurfFinish} }
            interior_texture{ InterTexture finish {InterFinish} }                                   

            scale <1,MaxY,1> // final scale
        }     /// mesh2


//        #declare ShiftXY=-0.05*(SceneMinX+SceneMaxX)/2;
   #end   // if (Nx, Ny)

#end    // macro DrawSurfaceOnBlock(DataFileName)...

/****f* Mesh2Surf.inc/DrawFuzzySurface ()
* PURPOSE
*  Draws surface representation in the form of smooth mesh of triangles
* SYNOPSIS
DrawSurface(string filename DataFileName)
* INPUTS
*  DataFileName - valid name of file with data - see example file for details
*  of its structure. Generally speaking first is Xaxis title, Yaxis title,
*  number of columns, then min and max value assigned to the items in first and last X column respectively (along POVRay X axis),
*  umber of rows, then min and max value assigned to the items in first and last row respectively (along POVRay -Z axis),
*  eventually numeric data in form of matrix, ie. data measured, observed or
*  calculated over the real XY grid
* SEE ALSO
*  WireSurf.inc
*  BPatchSurf.inc
*  DrawSurfaceOnBlock ()
****/
#macro DrawFuzzySurface(DataFileName)
// At present MinX := x1 !!!
    #fopen DataFile DataFileName read
    #read(DataFile, DescriptX, DescriptY)
    #read(DataFile, Nx, x1, x2)
    #read(DataFile, Ny, y1, y2)
    #if ((Nx>1)&(Ny>1))

        #declare Nodes=array[Ny][Nx];          // float: height of nodes only

        #declare QNodes=array[Nx*Ny+(Nx-1)*(Ny-1)];      // float vectors: full mesh2 "vertices"
        #declare QNodes[0]=<0,0,0>;                      // explicit typing QNode as vectors

        #declare Pointers=array[Ny][Nx];                 // integer pointers to QNodes array/temporary
        #declare Pointers[0][0]=0;                       // QNodes[Pointers[i][j]]=> node Nx,Ny
                                                         // QNodes[nx*Y+X]=>Nodes[X,Y]

        #declare Quads=array[Ny-1][Nx-1][5];             // quintet of integer pointers to corners and center of rectangles

//        #declare QNodesC=array[(Nx-1)*(Ny-1)];         // float: height of centers of rectangles
//        #declare Triangs=array[4*(Nx-1)*(Ny-1)][3]; // triplets of integer pointers to vertices of triangles

        #declare deltaX=(x2-x1)/(Nx-1);
        #declare deltaY=(y2-y1)/(Ny-1);
//#debug concat("deltaX= ",str(deltaX,5,3), ";  deltaY= ",str(deltaY,5,3) "\n********\n\n")
        #declare CountY=0;
        #declare Nodes[0][0]=0.0;                       // explicit typing array as float

        #declare MinY = 0.0;
        #declare MaxY = 0.0;     /// this is BAD!

        #declare MinX =  x1;
        #declare MaxX =  x2;

        #declare MinZ = -y1; // remember of reversed POVRay coordinates
        #declare MaxZ = -y2; // remember of reversed POVRay coordinates

        #declare NRow = 1;

        #declare QSurfSide = sqrt(deltaX*deltaY);         // try to estimate nice values for
        #declare NodeRadii = NodeRadiiCoeff*QSurfSide;    // NodeRadiiCoeff &
        #declare WireRadii = WireRadiiCoeff*QSurfSide;    // WireRadiiCoeff

        #debug concat("Just reading ", DataFileName,"\n")
        #local JustRead=0.0;
        #declare CountY=0;
        #while(CountY<Ny)
          #declare CountX=0;
          #while(CountX<Nx)
        //#debug concat("\n\n********\nCountX= ",str(CountX,5,0), "\n")
        //#debug concat("CountY= ",str(CountY,5,0), "\n********\n\n")
        //#debug DataFileName
               #read(DataFile, JustRead) 
               #if (JustRead < MinY)
                 #declare MinY = JustRead;
               #end

               #if (JustRead > MaxY)
                 #declare MaxY = JustRead;
               #end
             #declare Nodes[CountY][CountX]=JustRead;
             #declare CountX=CountX+1;
          #end
          #declare CountY=CountY+1;
        #end
        #fclose DataFile
        
// here search for MinY, MaxY
//
//
// end        

// matrix data just read in
// Lets analyse them....

//        #debug concat("SceneMaxY= ",str(SceneMaxY,10,7), "\n********\n\n")
        #declare DscMaxY=MaxY;
        #declare MinY=YScaleFactor*MinY;
        #declare MaxY=YScaleFactor*MaxY; // added 2012-11-10
        #declare SceneMinY=MinY;
        #declare SceneMaxY=MaxY;
        #declare SceneMinX=x1;
        #declare SceneMaxX=x2;
        #declare SceneMinZ=-y1;
        #declare SceneMaxZ=-y2;

// Now transform Nodes(x,y)=>QNodes(i)<>
        #declare CountY=0;
        #declare QCount=0; // QNodes index
        #while(CountY<Ny)
          #declare CountX=0;
          #while(CountX<Nx)
             #declare QNodes[CountY*Nx+CountX]=<x1+CountX*deltaX, Nodes[CountY][CountX], -(y1+CountY*deltaY)>; // "real world" coordinates
             #declare Pointers[CountY][CountX]=QCount;

             #declare CountX=CountX+1;
             #declare QCount=QCount+1;
          #end
          #declare CountY=CountY+1;
        #end

//Central points

        #declare QuaN=0; // Quads counter
        #declare CountY=0;
        #while(CountY<Ny-1)
          #declare CountX=0;
          #while(CountX<Nx-1)
            #declare IndexA=Pointers[CountY][CountX];           //  A****B
            #declare IndexB=Pointers[CountY][CountX+1];         //  *    *
            #declare IndexC=Pointers[CountY+1][CountX+1];       //  *    *
            #declare IndexD=Pointers[CountY+1][CountX];         //  D****C

            #declare QNodes[QCount]=(<QNodes[IndexA].x, QNodes[IndexA].y, QNodes[IndexA].z>+
                                     <QNodes[IndexB].x, QNodes[IndexB].y, QNodes[IndexB].z>+
                                     <QNodes[IndexC].x, QNodes[IndexC].y, QNodes[IndexC].z>+
                                     <QNodes[IndexD].x, QNodes[IndexD].y, QNodes[IndexD].z>)
                                     /4;
  //      #debug concat("CountY= ",str(CountY,5,0), "   CountX= ",str(CountX,5,0),  "\n********\n\n")
            #declare Quads[CountY][CountX][0]=IndexA; //
            #declare Quads[CountY][CountX][1]=IndexB;
            #declare Quads[CountY][CountX][2]=IndexC;
            #declare Quads[CountY][CountX][3]=IndexD;
            #declare Quads[CountY][CountX][4]=QCount;

            #declare QuaN=QuaN+1; //next
            #declare CountX=CountX+1;
            #declare QCount=QCount+1;
          #end
          #declare CountY=CountY+1;
        #end


        #if (ShowPoints)       // need to parametrize texture as a separate object
            #declare Count=0;
            #while(Count<Ny*Nx+(Nx-1)*(Ny-1))
                   sphere {
                           #declare YYY=YScaleFactor*QNodes[Count].y+Tiny;
                          <QNodes[Count].x, YYY, -QNodes[Count].z>, NodeRadii

                          material {
                          texture {
                            pigment {rgbt <QNodes[Count].y/DscMaxY,.0,1-QNodes[Count].y/DscMaxY,  0.3>}
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
                            fade_color <QNodes[Count].y/DscMaxY,.5,1-QNodes[Count].y/DscMaxY>
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
               #declare Count=Count+1;
            #end
        #end
        union{
// number of triangles: = 4*(Nx-1)*(Ny-1);
            #declare CountY=0;
            #while(CountY<Ny-1)
              #declare CountX=0;
              #while(CountX<Nx-1)         
              
                  DrawFuzzyTriangle2( <QNodes[Quads[CountY][CountX][4]].x, YScaleFactor*QNodes[Quads[CountY][CountX][4]].y+Tiny, QNodes[Quads[CountY][CountX][4]].z>, 
                                      <QNodes[Quads[CountY][CountX][1]].x, YScaleFactor*QNodes[Quads[CountY][CountX][1]].y+Tiny, QNodes[Quads[CountY][CountX][1]].z>, 
                                      <QNodes[Quads[CountY][CountX][0]].x, YScaleFactor*QNodes[Quads[CountY][CountX][0]].y+Tiny, QNodes[Quads[CountY][CountX][0]].z>, 
                                      .01 )
                                      
                  DrawFuzzyTriangle2( <QNodes[Quads[CountY][CountX][4]].x, YScaleFactor*QNodes[Quads[CountY][CountX][4]].y+Tiny, QNodes[Quads[CountY][CountX][4]].z>, 
                                      <QNodes[Quads[CountY][CountX][2]].x, YScaleFactor*QNodes[Quads[CountY][CountX][2]].y+Tiny, QNodes[Quads[CountY][CountX][2]].z>, 
                                      <QNodes[Quads[CountY][CountX][1]].x, YScaleFactor*QNodes[Quads[CountY][CountX][1]].y+Tiny, QNodes[Quads[CountY][CountX][1]].z>, 
                                      .01 )
                                      
                  DrawFuzzyTriangle2( <QNodes[Quads[CountY][CountX][4]].x, YScaleFactor*QNodes[Quads[CountY][CountX][4]].y+Tiny, QNodes[Quads[CountY][CountX][4]].z>, 
                                      <QNodes[Quads[CountY][CountX][3]].x, YScaleFactor*QNodes[Quads[CountY][CountX][3]].y+Tiny, QNodes[Quads[CountY][CountX][3]].z>, 
                                      <QNodes[Quads[CountY][CountX][2]].x, YScaleFactor*QNodes[Quads[CountY][CountX][2]].y+Tiny, QNodes[Quads[CountY][CountX][2]].z>, 
                                      .01 )
                                      
                  DrawFuzzyTriangle2( <QNodes[Quads[CountY][CountX][4]].x, YScaleFactor*QNodes[Quads[CountY][CountX][4]].y+Tiny, QNodes[Quads[CountY][CountX][4]].z>, 
                                      <QNodes[Quads[CountY][CountX][0]].x, YScaleFactor*QNodes[Quads[CountY][CountX][0]].y+Tiny, QNodes[Quads[CountY][CountX][0]].z>, 
                                      <QNodes[Quads[CountY][CountX][3]].x, YScaleFactor*QNodes[Quads[CountY][CountX][3]].y+Tiny, QNodes[Quads[CountY][CountX][3]].z>, 
                                      .01 )
                                      
                #declare CountX=CountX+1;
              #end
              #declare CountY=CountY+1;
            #end 
        
//            texture{ SurfTexture finish { SurfFinish }}
//            interior_texture{ InterTexture finish { InterFinish }}                                   
//            scale <1, MaxY, 1>
        }
        
//        #declare shiftXY = -0.05*(SceneMinX+SceneMaxX)/2; // Attempts to automatically determinate initial X description position
   #end   // if (Nx, Ny)

#end    // macro DrawFuzzySurface(DataFileName)...
                                      
/**************************************/






#end    // ifndef Mesh2Surf_INC...
