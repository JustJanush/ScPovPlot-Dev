/****h* ScPovPlot3D/CoordSys.inc
* PURPOSE
*   New templates for drawing coordination systems
*   Test field of different Coords visualization: Box, WiredBox, Plates, ...
*   It is the toolkit - usually not called by final user
*     image:./imgs/CoordSysS.png
*     |html <hr width=50% align="left">
*
*   Fig.[CoordsSys] New, wireframe coord system
*   There is a lot TODO: some automation and so on
*   |html <hr width=50% align="left">
*     *********************************************************
*     **   Tested on PovRay 3.7                              **
*     **   License: GNU GPL                                  **
*     **   Homepage:    http://scpovplot3d.sourceforge.net   **
*     *********************************************************
*     **   version: 3.0.6 (& have a nice time ;)             **
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


// Test field of different Coords: Box, WiredBox, Plates,
/// toolkit macro - usually not called by final user
/// API macro     - designed to be called by final user
/// Xp, Yp, Zp    - scene coordinates
/// Xr, Yr, Zr    - real world coords.

#ifndef(CoordsSys_INC)
#declare CoordsSys_INC = "2015-12-11"; // last modification date
#version 3.7;
#local _FileVerMaj   = "3.1.0.2";
#local _FileVerMin   = CoordsSys_INC;
#local _FileName     = "CoordsSys.inc";

#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

#ifndef (Debug) // Just 4 safety
   #declare Debug=0;
#end

// a lot of stone textures: T_Stone1 - T_Stone44
// Standard pre-defined colors
#include "math.inc"
#include "colors.inc"
#include "textures.inc"
#include "stones1.inc"

#include "CommonDefs.inc"
#include "Cameras.inc"

#declare RCornerSize=1.0;
#macro SetCornerSize(_alf)
  #declare RCornerSize=_alf;
#end

/// #macro ThickPlateBox - coord system witch thick walls
/// _D - thickness of the walls
/// _dX, _dY, _dZ length of axes in absolute units (provisionary now)
///  Real world units !!!! internaly converted into PovRay-system
///  Plus, Plus, Plus axes half - standard upper main quarter

#macro ThickPlateBoxPPP (_D) //, _dX, _dY, _dZ)
difference{

  box{<-_D, -_D, _D>, P111 }//<_dY, _dZ, -_dX> }

  box{<0,0,0>, P111+Tiny*<1,1,-1> }

}
#end


///  Minus, Minus, Minus - axes half - standard lower minus quarter
#macro ThickPlateBoxMMM (_D)//, _dX, _dY, _dZ)
difference{

  box{<_D, _D, -_D>, <-_dY, -_dZ, _dX> }

  box{<0,0,0>, <-_dY-Tiny, -_dZ-Tiny, _dX+Tiny> }

}
#end


////
/// #macro WirePlate(_dY, _dZ, _R)
/// rectangular wire frame
/// _dY, _dZ, x,z  real size of frame
/// _R radius ofthe wire
#macro WirePlate(_dY, _dZ, _R)
union{
 sphere{ 0, _R}
 sphere{ <_dY, _dZ, 0>, _R}
 sphere{ <0, _dZ, 0>, _R}
 sphere{ <_dY, 0, 0>, _R}
 cylinder{0, <0, _dZ, 0>, _R }
 cylinder{0, <_dY, 0, 0>, _R }
 cylinder{<_dY, _dZ, 0>, <0, _dZ, 0>, _R }
 cylinder{<_dY, _dZ, 0>, <_dY, 0, 0>, _R }

}
#end


///
/// #macro WiredBoxBase(_dY, _dZ, _R) main quarter of coord sys.
/// depends on previous call to PrepareCamera() macro !!!
/// rectangular wire frame
/// _dX, _dY, _dZ, x,y, z  !!real size of frame!!
/// _R - radius ofthe wire
/// _N - number of internal grid lines...
#macro WiredBoxBase(_dX, _dY, _dZ, _R, _N)
#local P000 = <0,0,0>;
#local P111 = <sgn(camera_x)* _dY, sgn(camera_y)*_dZ, sgn(camera_z)*_dX >;

#local P100 = <P111.x,    0,    0>;
#local P010 = <   0, P111.y,    0>;
#local P001 = <   0,    0, P111.z>;
#local P101 = P100+P001;
#local P110 = P100+P010;
#local P011 = P010+P001;

#local _Rr=_R*RCornerSize; // larger spheres at corners

union{
 sphere{ P000, _Rr}
 sphere{ P111, _Rr}
 sphere{ P100, _Rr}
 sphere{ P010, _Rr}
 sphere{ P001, _Rr}
 sphere{ P110, _Rr}
 sphere{ P011, _Rr}
 sphere{ P101, _Rr}


 cylinder{P000, P010, _R }
 cylinder{P000, P100, _R }

 cylinder{P110, P100, _R }
 cylinder{P110, P010, _R }

 cylinder{P111, P101, _R }
 cylinder{P111, P011, _R }

 cylinder{P001, P011, _R }
 cylinder{P001, P101, _R }

 cylinder{P000, P001, _R }
 cylinder{P010, P011, _R }
 cylinder{P110, P111, _R }
 cylinder{P100, P101, _R }
}
#end

/* Real coords (left) vs PovRay Coords (right)
     ^                        ^
    Z|                       Y|
     |                        |
     |____ Y                  |____ X
    /                        /
   /X                       /-Z
*/

/// Initially all charts are positioned at the XYZ coord system origin
/// then, chart is moved to final scene position maybe scaled.
/// real origin of the chart is a matter of description on axes rather than
/// placement of coords marks ie. walls, plates and so on.
/// Such a chart decorations shouldn't interfere with chart itself
/// thus have to be placed _around_ the chart.

/// toolkit macro
/// #macro _SetCoordBoxData(_dX, _dY, _dZ)
/// defines main corner points of coord sys., I mean origin
/// and other as well :P Some of them will be used to create
/// chart decorations
/// _dX, _dY, _dZ, - dimensions in scene units [SU]
#macro _SetCoordBoxData(_dX, _dY, _dZ) // Always the same points
    #declare P000 = <0,0,0>;
    #declare P111 = <_dY, _dZ, -_dX>;

    #declare P100 = <P111.x,    0,    0>;
    #declare P010 = <   0, P111.y,    0>;
    #declare P001 = <   0,    0, P111.z>;
    #declare P101 = P100+P001;
    #declare P110 = P100+P010;
    #declare P011 = P010+P001;

#end

/// toolkit macro
/// It is suite of 8 macros from **PPP to **MMM
/// depending of camera position, some walls are visible or not
/// #macro WiredBoxPPP(_dY, _dZ, _R) main quarter of coord sys.
/// PPP - Plus X, Plus Y, Plus Z (real coords)
/// depends on previous call to PrepareCamera() macro !!!
/// rectangular wire frame
/// _R - radius of the wire
/// _Nh, _Nv - number of internal grid lines h-orizonta & v-ertical...
#macro WiredBoxPPP(_R, _Nh, _Nv)

#local _Rr=_R*RCornerSize; // enlarge spheres at corners

union{
 sphere{ P000, _Rr}
// sphere{ P111, _Rr} this sphere isn't visible
 sphere{ P100, _Rr}
 sphere{ P010, _Rr}
 sphere{ P001, _Rr}
 sphere{ P110, _Rr}
 sphere{ P011, _Rr}
 sphere{ P101, _Rr}

 cylinder{P000, P010, _R }
 cylinder{P000, P100, _R }

 cylinder{P110, P100, _R }
 cylinder{P110, P010, _R }

 cylinder{P001, P011, _R }
 cylinder{P001, P101, _R }

 cylinder{P000, P001, _R }
 cylinder{P010, P011, _R }
 cylinder{P100, P101, _R }

// now horiz. grid on Xp-Yp plane
#local vvecd=(_Nv>0?(P010-P000)/_Nv:P010);
#local c0 = P000;
#local c1 = P100;
#for(nnn, 1, _Nv-1)
 cylinder{c0+nnn*vvecd, c1+nnn*vvecd, _R/2 }
#end

// now vert. grid on Xp-Yp plane
#local vvecdX=(_Nh>0?(P100-P000)/_Nh:P100);
#local c1 = P010;
#for(nnn, 1, _Nh-1)
 cylinder{c0+nnn*vvecdX, c1+nnn*vvecdX, _R/2 }
#end

// now horiz. grid on Zp-Yp plane
#local vvecd=(_Nv>0?(P010-P000)/_Nv:P010);
#local c1 = P001;
#for(nnn, 1, _Nv-1)
 cylinder{c0+nnn*vvecd, c1+nnn*vvecd, _R/2 }
#end

// now vert. grid on Zp-Xp plane
#local vvecdZ=(_Nh>0?(P001-P000)/_Nh:P100);
#local c1 = P010;
#for(nnn, 1, _Nh-1)
 cylinder{c0+nnn*vvecdZ, c1+nnn*vvecdZ, _R/2 }
#end

// eventally base plane:
//
#local c1 = P100;
#for(nnn, 1, _Nh-1)
 cylinder{c0+nnn*vvecdZ, c1+nnn*vvecdZ, _R/2 }
#end

#local c1 = P001;
#for(nnn, 1, _Nh-1)
 cylinder{c0+nnn*vvecdX, c1+nnn*vvecdX, _R/2 }
#end

}
#end


//   POVRay   => real world
//  x - green => Y real
//  y - blue  => Z real
// -z - red   => X real

#macro InsertCartesian()
union {
   cylinder {<-1/3, .0, .0>, <1, 0, 0>, .01 hollow  texture{ pigment {color <0,1.5,0>}}} //  X-pov axis/y real
   cylinder {< .0,-1/3, .0>, <0, 1, 0>, .01 hollow  texture{ pigment {color <0,0,1.1>}}} //  Y-pov axis/z real
   cylinder {< .0, .0, 1/3>, <0, 0,-1>, .01 hollow  texture{ pigment {color <1.5,0,0>}}} // -Z-pov axis/x real
   scale 2.5
   translate <0, 0, 0> // translate if needed (dx, dy, dz)
}
#end

#macro InsertCartesian_LD(L,D)
union {
   cylinder {<-L/3, .0, .0>, <L,0, 0>, D/2 hollow  texture{ pigment {color <0,1.5,0>}}} // X-pov axis
   cylinder {< .0,-L/3, .0>, <0,L, 0>, D/2 hollow  texture{ pigment {color <0,0,1.1>}}} // Y-pov axis
   cylinder {< .0, .0, L/3>, <0,0,-L>, D/2 hollow  texture{ pigment {color <1.5,0,0>}}} // -Z-pov axis
   translate <0, 0, 0> // translate if needed (dx, dy, dz)
}
#end

#macro InsertCartesianArrows_LD(L, D, Zf)  // additional Z axis scaling
union {
   merge {
       cylinder { <.0, .0, L/3>, <0,0, -L>, D/2 } // X-real axis   '-z-pov'
       cone { <0,0,-L>,D*1.5,<0,0,-L-4*D>,0 }      // end of cone -------------------------------------
          texture { pigment{ color rgb<1.5, 0, 0>} finish { phong 1 reflection{ 0.00 metallic 0.00} } } // end of texture
   }

   merge {
       cylinder {< -L/3, 0, .0>, <L, 0, 0>, D/2 }     // Y-real axis
       cone { <L,0,0>,D*1.5,<L+4*D,0,0>,0 }           // end of cone -------------------------------------
          texture { pigment{ color rgb<0,1.5,0>} finish { phong 1 reflection{ 0.00 metallic 0.00} } } // end of texture
   }

   #if(Zf > Tiny) // do not draw if too short...
   merge {
       cylinder {< .0, -Zf*L/3, 0>, <0, Zf*L, 0>, D/2 } // Z-real axis
       cone { <0, Zf*L, 0>, D*1.5,<0, Zf*L+4*D, 0>,0 }   // end of cone -------------------------------------
          texture { pigment{ color rgb<0, 0, 1.5>} finish { phong 1 reflection{ 0.00 metallic 0.00} } } // end of texture
   }    
   #end
}
#end

#macro InsertCartesianArrows_LDs(L, D, Hf, Zf)  // additional HORIZONTAL(Hf) and VERTICAL(Zf) axis scaling
union {
   merge {
       cylinder { <.0, .0, L/3>, <0,0, -L>, D/2 } // X-real axis   '-z-pov'
       cone { <0,0,-L>,D*1.5,<0,0,-L-4*D>,0 }      // end of cone -------------------------------------
          texture { pigment{ color rgb<1.5, 0, 0>} finish { phong 1 reflection{ 0.00 metallic 0.00} } } // end of texture
   }

   merge {
       cylinder {< -1/*Hf*/*L/3, 0, .0>, <Hf*L, 0, 0>, D/2 }     // Y-real axis
       cone { <Hf*L,0,0>,D*1.5,<Hf*L+4*D,0,0>,0 }           // end of cone -------------------------------------
          texture { pigment{ color rgb<0,1.5,0>} finish { phong 1 reflection{ 0.00 metallic 0.00} } } // end of texture
   }

   merge {
       cylinder {< .0, -1/*Zf*/*L/3, 0>, <0, Zf*L, 0>, D/2 } // Z-real axis
       cone { <0, Zf*L, 0>, D*1.5,<0, Zf*L+4*D, 0>,0 }   // end of cone -------------------------------------
          texture { pigment{ color rgb<0, 0, 1.5>} finish { phong 1 reflection{ 0.00 metallic 0.00} } } // end of texture
   }
}
#end


///**************************
/// #macro InsertCartesianArrows_LDW(L, D, Zf)
///    scaled axis with Coord system base planes (simple)
/// L:
/// D:
/// Zf:
///==========================
#macro InsertCartesianArrows_LDW(L, D, Zf)  // scaled axis with Coord system base planes (simple)
union{
     InsertCartesianArrows_LD(L, D, Zf)
     #declare YL = Zf*L;
     polygon { // XY plane aka -Z*X _pov
       5,
          <-L/3, 0, L/3>, <L, 0, L/3>, <L, 0, -L>, <-L/3, 0, -L>, <-L/3, 0, L/3>
       texture { finish { reflection {.13 metallic} diffuse 1.,.50 ambient 0.41 }
             pigment { color rgbft<1.0, 1.0, 1.0, .10, .79> }
       }
     }
     polygon { // XZ plane aka  -Z*Y _pov
       5,
          <0, -YL/3, L/3>, <0, -YL/3, -L>, <0, YL, -L>, <0, YL, L/3>, <0, -YL/3, L/3>
       texture { finish { reflection {.13 metallic} diffuse 1.,.50 ambient 0.41 }
             pigment { color rgbft<1.0, 1.,1.0, .10, .79> }
       }
     }

     polygon { // YZ plane aka X*Y _pov
       5,
          <-L/3, -YL/3, 0>, <L, -YL/3, 0>, <L, YL, 0>, <-L/3, YL, 0>, <-L/3, -YL/3, 0>
       texture { finish { reflection {.13 metallic} diffuse 1.,.5 ambient 0.41 }
             pigment { color rgbft<1.0, 1.0, 1.0, .1, .79> }
        }
     }
}
#end

///**************************
/// #macro InsertCartesianArrows_LDWT(L, D, Hf, Zf,  _Txtr)
///    scaled axis with Coord system base planes (simple)
/// L:     base length of axes
/// D:     diameter of axes
/// Hf:    horizontal scaling factor
/// Zf:    vertical scaling factor
/// Txtr:  texture definede elswhere
///==========================
#macro InsertCartesianArrows_LDWT(L, D, Hf, Zf, _Txtr)  // scaled axis with Coord system base planes (simple)
union{
     InsertCartesianArrows_LDs(L, D, Hf, Zf)
     #local YL = Zf*L;
     #local XL = Hf*L;
     merge{
         polygon { // XY plane aka -Z*X _pov
              5, <-XL/3, 0, L/3>, <XL, 0, L/3>, <XL, 0, -L>, <-XL/3, 0, -L>, <-XL/3, 0, L/3> }
         polygon { // XZ plane aka  -Z*Y _pov
              5, <0, -YL/3, L/3>, <0, -YL/3, -L>, <0, YL, -L>, <0, YL, L/3>, <0, -YL/3, L/3> }
         polygon { // YZ plane aka X*Y _pov
              5, <-XL/3, -YL/3, 0>, <XL, -YL/3, 0>, <XL, YL, 0>, <-XL/3, YL, 0>, <-XL/3, -YL/3, 0> }
         texture { _Txtr      }
     }
}
#end



///**************************
/// #macro InsertCartesianWalls(fL, Zf, AdSpace)
///    scaled Coord system's base planes
/// insert cartesian walls around cart
/// fL: scaling horizontal factor, usually fL=1 but sometimes fl>=1, for example 1.1
/// Zf: scaling vertical factor, usually Zf=1 but sometimes Zf<1, for example 0.5
/// AdSpace: additional space _above_ coord-sys, due to shift of XY_R base down along vertical axis
///==========================
#macro InsertCartesianWalls(fL, Zf, AdSpace)  // scaled Coord system's base planes
union{

     #declare YL = Zf*SceneMaxY+AdSpace; // Y_pov vertical size
     #declare XL = fL*SceneMaxX; // X_pov horizontal size
     #declare ZL = -fL*SceneMaxZ; // Z_pov horizontal size

     polygon { // XY plane aka -Z*X _pov
       5,
          <-XL/3, 0, ZL/3>, <XL, 0, ZL/3>, <XL, 0, -ZL>, <-XL/3, 0, -ZL>, <-XL/3, 0, ZL/3>
       texture { finish { reflection {.13 metallic} diffuse 1.,.50 ambient 0.41 }
             pigment { color rgbft<1.0, 1.0, 1.0, .10, .79> }
       }
     }
     polygon { // XZ plane aka  -Z*Y _pov
       5,
          <0, -YL/3, ZL/3>, <0, -YL/3, -ZL>, <0, YL, -ZL>, <0, YL, ZL/3>, <0, -YL/3, ZL/3>
       texture { finish { reflection {.13 metallic} diffuse 1.,.50 ambient 0.41 }
             pigment { color rgbft<1.0, 1.,1.0, .10, .79> }
       }
     }

     polygon { // YZ plane aka X*Y _pov
       5,
          <-XL/3, -YL/3, 0>, <XL, -YL/3, 0>, <XL, YL, 0>, <-XL/3, YL, 0>, <-XL/3, -YL/3, 0>
       texture { finish { reflection {.13 metallic} diffuse 1.,.5 ambient 0.41 }
             pigment { color rgbft<1.0, 1.0, 1.0, .1, .79> }
        }
     }



}
#end



// auto scaled axis with Coord system base planes _Zsc, _Ysc, Vertical and Y axe scaling factors
#macro InsertAutoCartesianArrows(_DL, _D, _Zsc, _Ysc)
#declare Zf=_Zsc;
union{
union {
   merge {
       #declare L=-SceneMaxZ+_DL;
       cylinder { <.0, .0, L/3>, <0,0, -L>, _D/2 } // X-real axis   '-z -pov'
       cone { <0,0,-L>,_D*1.5,<0,0,-L-4*_D>,0 }      // end of cone -------------------------------------
          texture { pigment{ color rgb<1.5, 0, 0>} finish { phong 1 reflection{ 0.00 metallic 0.00} } } // end of texture
   }

   merge {
       #declare L=SceneMaxX+_DL;
       cylinder {< -L/3, 0, .0>, <L, 0, 0>, _D/2 }     // Y-real axis
       cone { <L,0,0>,_D*1.5,<L+4*_D,0,0>,0 }           // end of cone -------------------------------------
          texture { pigment{ color rgb<0,1.5,0>} finish { phong 1 reflection{ 0.00 metallic 0.00} } } // end of texture
   }

   merge {
       #declare L=SceneMaxY+_DL;
       cylinder {< .0, -Zf*L/3, 0>, <0, Zf*L, 0>, _D/2 } // Z-real axis
       cone { <0, Zf*L, 0>, _D*1.5,<0, Zf*L+4*_D, 0>,0 }   // end of cone -------------------------------------
          texture { pigment{ color rgb<0, 0, 1.5>} finish { phong 1 reflection{ 0.00 metallic 0.00} } } // end of texture
   }
}

     #declare YL =  _Zsc*(SceneMaxY+_DL);
     #declare Lz =  -SceneMaxZ+_DL;
     #declare Lx =  _Ysc*(SceneMaxX+_DL);

     polygon { // XY plane aka -Z*X _pov
       5,
          <-Lx/3, 0, Lz/3>, <Lx, 0, Lz/3>, <Lx, 0, -Lz>, <-Lx/3, 0, -Lz>, <-Lx/3, 0, Lz/3>
       texture { finish { reflection {.13 metallic} diffuse 1.,.50 ambient 0.41 }
             pigment { color rgbft<1.0, 1.0, 1.0, .10, .79> }
       }
     }
     polygon { // XZ plane aka  -Z*Y _pov
       5,
          <0, -YL/3, Lz/3>, <0, -YL/3, -Lz>, <0, YL, -Lz>, <0, YL, Lz/3>, <0, -YL/3, Lz/3>
       texture { finish { reflection {.13 metallic} diffuse 1.,.50 ambient 0.41 }
             pigment { color rgbft<1.0, 1.,1.0, .10, .79> }
       }
     }

     polygon { // YZ plane aka X*Y _pov
       5,
          <-Lx/3, -YL/3, 0>, <Lx, -YL/3, 0>, <Lx, YL, 0>, <-Lx/3, YL, 0>, <-Lx/3, -YL/3, 0>
       texture { finish { reflection {.13 metallic} diffuse 1.,.5 ambient 0.41 }
             pigment { color rgbft<1.0, 1.0, 1.0, .1, .79> }
        }
     }



}
#end


/****************************************************/
/****************************************************/
//  Base plate and sidewalls
#macro AddBasePlate(D)
    box{
       <SceneMinX-D, SceneMinY-D, SceneMinZ>, <SceneMaxX, SceneMinY-Tiny, -SceneMaxZ>
       texture { pigment{color tmpColour}}
    }
#end


// .1, .13, .05, 0.8, 0.2
#macro AddBasePlateRGBFT(D, RR, BB, GG, FF, TT)
   box{
       <SceneMinX-D, SceneMinY-D, SceneMinZ+D>, <SceneMaxX, SceneMinY-Tiny, SceneMaxZ>
       texture { pigment{color rgbft <RR, BB, GG, FF, TT>}}
   }
#end

// it should depend on camera position, so .. go on!
#macro AddSideWalls(D)
merge{
        box { <SceneMinX-D, SceneMinY, SceneMinZ> , <SceneMaxX, 1.1*SceneMaxY, SceneMinZ+D>  }
        box { <-SceneMinZ-D, SceneMinY-Tiny, SceneMinX-D> , <-SceneMaxZ, 1.1*SceneMaxY+Tiny, SceneMinX>
          rotate y*90
        }
        texture  { tmpTexture }
        interior { tmpInterior }
  }
#end

//obsolete!
#macro AddSideWallsRGBFT(D, RR, BB, GG, FF, TT)
#debug "Macro 'AddSideWallsRGBFT()' tends to be obsolete, use Add###Plate(Thickness)"
merge{
        box { <SceneMinX-D, SceneMinY, SceneMinZ> ,  <SceneMaxX, 1.1*SceneMaxY, SceneMinZ+D>  }
        box { <-SceneMinZ-D, SceneMinY-Tiny, SceneMinX-D> ,  <-SceneMaxZ, 1.1*SceneMaxY+Tiny, SceneMinX>
          rotate y*90
        }
        texture  { tmpTexture }
        interior { tmpInterior }
  }
#end

#macro AddBaseAndSidePlatesRGB(D)
merge{
       box{
           <SceneMinX, SceneMinY-D, SceneMinZ+D>, <SceneMaxX, SceneMinY-Tiny, SceneMaxZ>
           texture { pigment{color tmpColour}}
           // rgb - Red Green Blue
       }

       merge{ box { <SceneMinX-D, SceneMinY-D, SceneMinZ+D> ,  <SceneMaxX, 1.1*SceneMaxY, SceneMinZ>  }//t³o

              box { <-SceneMinZ-D, SceneMinY-D-Tiny, SceneMinX-D> ,  <-SceneMaxZ, 1.1*SceneMaxY+Tiny, SceneMinX>
                    rotate y*90
              }
            texture  { tmpTexture }
            interior { tmpInterior }
       }
  }
#end

#macro AddBaseAndSidePlatesRGBFT(D)
merge{
       box{
           <SceneMinX-D, SceneMinY-D, SceneMinZ+D>, <SceneMaxX, SceneMinY-Tiny, SceneMaxZ>
           texture { pigment{color rgbft <RR, BB, GG, FF, TTT>}}
       }
       merge{ box { <SceneMinX-D, SceneMinY-Tiny, 0> ,  <SceneMaxX, 1.1*SceneMaxY+Tiny, D>  }//t³o

              box { <-SceneMinZ, SceneMinY, -D> ,  <-SceneMaxZ, 1.1*SceneMaxY, 0>
                    rotate y*90
              }
            texture  { tmpTexture }
            interior { tmpInterior }
       }
  }
#end

// _Image is to be drawn over X-Y plane
#macro AddBaseAndSidePlatesRGB_Image(D, _Image, _Filter, _Power, Inter)
#ifdef (Debug)
  #debug concat("maxX= ", str(SceneMaxX, 10, 6),"\n")
  #debug concat("minX= ", str(SceneMinX, 10, 6),"\n")

  #debug concat("maxY= ", str(SceneMaxY, 10, 6),"\n")
  #debug concat("minY= ", str(SceneMinY, 10, 6),"\n")

  #debug concat("maxZ= ", str(SceneMaxZ, 10, 6),"\n")
  #debug concat("minZ= ", str(SceneMinZ, 10, 6),"\n")
#end

union{
       box{  // ZX - base plate
           <SceneMinX-D, SceneMinY-D, SceneMinZ+D>, <SceneMaxX, SceneMinY-Tiny, SceneMaxZ>
           texture { pigment{color tmpColour}}
           // rgb - Red Green Blue
       }

       union{  // XY plate
           union{
                polygon {
                    5,
                    #if (TilXYimage)
                        <0, 0>, <0, 1.1*(SceneMaxY-SceneMinY)>, <SceneMaxX, 1.1*(SceneMaxY-SceneMinY)>, <SceneMaxX, 0>, <0, 0>
                    #else
                       <0, 0>, <0, 1>, <1, 1>, <1, 0>, <0, 0>
                    #end
                    // internal texture
                    #if ( __MattSet )
                       texture { __MattTexture }
                    #end
                    texture {
                      finish { ImgFinish }
                      pigment {
                           #if (file_exists(_Image))
                             #if (Debug)
                               #debug concat(endl, _FileName, ": [ext=",__ImageType, "]",endl)
                             #end
                             image_map { // exr | gif | hdr | iff | jpeg | pgm | png | ppm | sys | tga | tiff
                                #if (__ImageType="jpeg") jpeg #end
                                #if (__ImageType="tiff") tiff #end
                                #if (__ImageType= "png")  png #end
                                #if (__ImageType= "gif")  gif #end
                                #if (__ImageType= "iff")  iff #end
                                #if (__ImageType= "hdr")  hdr #end
                                #if (__ImageType= "exr")  exr #end
                                #if (__ImageType= "tga")  tga #end
                                #if (__ImageType= "pgm")  pgm #end
                                #if (__ImageType= "ppm")  ppm #end
                                #if (__ImageType= "sys")  sys #end
                                _Image     // actual file name

                                gamma
                                #if (ImgGamma>0) ImgGamma #else srgb #end
                                #if (!TilXYimage) once #end
                                #switch (Inter)
                                   #case (2) interpolate 2 #break // bilinear
                                   #case (3) interpolate 3 #break // bicubic
                                   #case (4) interpolate 4 #break // normalized
                                   #else #debug "image_map: enter 2 or 4" #break
                                #end
                                #switch (_Filter)
                                   #case (1) filter   all _Power #break  // filtered
                                   #case (2) transmit all _Power #break  // transmitted
                                   #else #debug "[!] - default setting for \'filter/transmit\' used. Good choice!\n" #break
                                #end
                                #if ( __ImgFilterSet )
                                      filter   all __ImgFilterPower
                                #end
                                #if ( __ImgTransmitSet )
                                      transmit   all __ImgTransmitPower
                                #end
                              }
                          #else
                              color rgb 0.5
                              #debug "Image specified not found"
                          #end
                         }
                      }
                      scale <SceneMaxX, 1.1*SceneMaxY, 1>
                      translate < SceneMinX, SceneMinY, SceneMinZ>
                    }
               box { <SceneMinX-D-Tiny, SceneMinY-Tiny, SceneMinZ+D> ,  <SceneMaxX, 1.1*SceneMaxY, SceneMinZ+Tiny>      // background wall X-Y
                     texture { tmpTexture } //finish { diffuse 0.85 phong 1 }
                     interior { tmpInterior }
               } // end of texture
             }

             box { // ZY wall
                 <-SceneMinZ-D-Tiny, SceneMinY-Tiny, SceneMinX-D> ,  <-SceneMaxZ, 1.1*SceneMaxY+Tiny, SceneMinX> // side wall Z-Y
                 rotate y*90
                 texture  { tmpTexture }
                 interior { tmpInterior }
             }
       }
  }
#end

#macro AddXYPlate(D)     // Done !
#ifdef (Debug)
  #debug concat("maxX= ", str(SceneMaxX, 10, 6),"\n")
  #debug concat("minX= ", str(SceneMinX, 10, 6),"\n")

  #debug concat("maxY= ", str(SceneMaxY, 10, 6),"\n")
  #debug concat("minY= ", str(SceneMinY, 10, 6),"\n")

  #debug concat("maxZ= ", str(SceneMaxZ, 10, 6),"\n")
  #debug concat("minZ= ", str(SceneMinZ, 10, 6),"\n")
#end

union{  // XY plate
      #if (strlen(XYimage)>0)
         polygon {
           5,
           #if (TilXYimage)
              <0, 0>, <0, 1.1*(SceneMaxY-SceneMinY)>, <SceneMaxX, 1.1*(SceneMaxY-SceneMinY)>, <SceneMaxX, 0>, <0, 0>
           #else
              <0, 0>, <0, 1>, <1, 1>, <1, 0>, <0, 0>
           #end
           // internal texture
           #if ( __MattSet )
              texture { __MattTexture }
           #end
           texture {
             finish { ImgFinish }
             pigment {
                     #if (file_exists(XYimage))
                       ImageType( XYimage  )
                       #if (Debug)
                         #debug concat(endl, "AddXYPlate ", XYimage,": ", _FileName, ": [ext=",__ImageType, "]",endl)
                       #end
                       image_map { // exr | gif | hdr | iff | jpeg | pgm | png | ppm | sys | tga | tiff
                          #if (__ImageType="jpeg") jpeg #end
                          #if (__ImageType="tiff") tiff #end
                          #if (__ImageType= "png")  png #end
                          #if (__ImageType= "gif")  gif #end
                          #if (__ImageType= "iff")  iff #end
                          #if (__ImageType= "hdr")  hdr #end
                          #if (__ImageType= "exr")  exr #end
                          #if (__ImageType= "tga")  tga #end
                          #if (__ImageType= "pgm")  pgm #end
                          #if (__ImageType= "ppm")  ppm #end
                          #if (__ImageType= "sys")  sys #end
                          XYimage     // actual file name

                          gamma
                          #if (ImgGamma>0) ImgGamma #else srgb #end
                          #if (!TilXYimage) once #end
                          #switch (ImgInter)
                             #case (2) interpolate 2 #break // bilinear
                             #case (3) interpolate 3 #break // bicubic
                             #case (4) interpolate 4 #break // normalized
                             #else #debug "image_map: enter 2 or 4" #break
                          #end
                          #switch (ImgFilter) // obsolete, will be removed
                             #case (1) filter   all ImgPower #break  // filtered
                             #case (2) transmit all ImgPower #break  // transmitted
                             #else #debug "[!] - default setting no \'filter/transmit\' used.\n" #break
                          #end
                          #if ( __ImgFilterSet )
                              filter   all __ImgFilterPower
                          #end
                          #if ( __ImgTransmitSet )
                              transmit   all __ImgTransmitPower
                          #end
                        }
                    #else
                        color rgb 0.5
                        #debug "Image specified not found"
                    #end

                }
             }
             #if (!TilXYimage)
                scale <SceneMaxX-SceneMinX, 1.1*(SceneMaxY-SceneMinY), 1>
             #end
             #if (camera_z<0)
                translate < SceneMinX, SceneMinY, SceneMinZ >
             #else
                rotate y*180
                translate <SceneMaxX, SceneMinY, SceneMaxZ+Tiny/2>
             #end



           }
      #end
///*
      box {
          #if (camera_z<0)
            <SceneMinX-(camera_x<0?0:D), -D, SceneMinZ+D> ,  <SceneMaxX+(camera_x<0?D:0), 1.1*(SceneMaxY-SceneMinY), SceneMinZ+Tiny>      // background wall X-Y
          #else
            <SceneMinX -(camera_x<0?0:D), -D, SceneMaxZ>, <SceneMaxX+(camera_x<0?D:0), 1.1*(SceneMaxY-SceneMinY), SceneMaxZ-D>      // background wall X-Y 2
          #end


      #switch (DeclareLevel)
        #case(0)  // apparently colour only is defined
             material{ texture { pigment{tmpColour1}
             #if (Effects)
                normal { bumps 0.5 scale 0.05} finish { diffuse 0.9 phong 1 }
             #end
                } // end of texture
             } // end of material -------------------
              #break
        #case(2) // apparently texture only is defined
             material{ texture { tmpTexture } // end of texture
             #if (Effects)
                normal { bumps 0.5 scale 0.05} finish { diffuse 0.9 phong 1 }
             #end
                    interior{ tmpInterior } // end of interior
               } // end of material -------------------
              #break
        #end
        translate<0,SceneMinY,0>
      } // */  // end of texture
     #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect on
        }
     #end
}
#end // XY


#macro AddXZPlate(D)
#ifdef (Debug)
  #debug concat("maxX= ", str(SceneMaxX, 10, 6),"\n")
  #debug concat("minX= ", str(SceneMinX, 10, 6),"\n")

  #debug concat("maxY= ", str(SceneMaxY, 10, 6),"\n")
  #debug concat("minY= ", str(SceneMinY, 10, 6),"\n")

  #debug concat("maxZ= ", str(SceneMaxZ, 10, 6),"\n")
  #debug concat("minZ= ", str(SceneMinZ, 10, 6),"\n")
#end

union{  // XZ plate = base plate
      #if (strlen(XZimage)>0)
         polygon {
           5,
           #if (TilXZimage)
              <0, 0, 0>, <0, -SceneMaxZ, 0>, <SceneMaxX, -SceneMaxZ, 0>, <SceneMaxX, 0, 0>, <0, 0>
           #else
              <0, 0, 0>, <0, 1, 0>, <1, 1, 0>, <1, 0, 0>, <0, 0, 0>
           #end
           // internal texture
           #if ( __MattSet )
              texture { __MattTexture }
           #end
           texture {
             finish { ImgFinish }
             pigment {
                     #if (file_exists(XZimage))
                       ImageType( XZimage  )
                       #if (Debug)
                         #debug concat(endl, "AddXZPlate ", XZimage,": ",_FileName, ": [ext=",__ImageType, "]",endl)
                       #end
                       image_map { // exr | gif | hdr | iff | jpeg | pgm | png | ppm | sys | tga | tiff
                          #if (__ImageType="jpeg") jpeg #end
                          #if (__ImageType="tiff") tiff #end
                          #if (__ImageType= "png")  png #end
                          #if (__ImageType= "gif")  gif #end
                          #if (__ImageType= "iff")  iff #end
                          #if (__ImageType= "hdr")  hdr #end
                          #if (__ImageType= "exr")  exr #end
                          #if (__ImageType= "tga")  tga #end
                          #if (__ImageType= "pgm")  pgm #end
                          #if (__ImageType= "ppm")  ppm #end
                          #if (__ImageType= "sys")  sys #end
                          XZimage     // actual file name

                          gamma
                          #if (ImgGamma>0) ImgGamma #else srgb #end
                          #if (!TilXZimage) once #end
                          #switch (ImgInter)
                             #case (2) interpolate 2 #break // bilinear
                             #case (3) interpolate 3 #break // bicubic
                             #case (4) interpolate 4 #break // normalized
                             #else #debug "image_map: enter 2 or 4" #break
                          #end
                          #switch (ImgFilter)
                             #case (1) filter   all ImgPower #break  // filtered
                             #case (2) transmit all ImgPower #break  // transmitted
                             #else #debug "[!] - default setting for \'filter/transmit\' used. Good choice!\n" #break
                          #end
                          #if (__ImgFilterSet)
                              filter   all __ImgFilterPower
                          #end
                          #if (__ImgTransmitSet)
                              transmit   all __ImgTransmitPower
                          #end
                        }
                    #else
                        color rgb 0.5
                        #debug "Image specified not found"
                    #end
                }
             }
             #if (!TilXZimage)
                scale <SceneMaxX-SceneMinX, SceneMinZ-SceneMaxZ, 1>
             #end
             rotate 90*x
             translate <SceneMinX, SceneMinY-Tiny/2, SceneMaxZ>
           }
      #end
      box { <SceneMinX-Tiny, SceneMinY-D, SceneMinZ+Tiny>,  <SceneMaxX, SceneMinY-Tiny, SceneMaxZ+Tiny>      // background wall X-Z-> Base plate is dimensioned without excess
      #switch (DeclareLevel)
        #case(0)  // apparently colour only is defined
             material{ texture { pigment{tmpColour}
             #if (Effects)
                normal { bumps 0.5 scale 0.05} finish { diffuse 0.9 phong 1 }
             #end
                } // end of texture
             } // end of material -------------------
              #break
        #case(2) // apparently texture only is defined
             material{ texture { tmpTexture } // end of texture
                 #if (Effects)
                    normal { bumps 0.5 scale 0.05} finish { diffuse 0.9 phong 1 }
                 #end
                 interior{ tmpInterior } // end of interior
             } // end of material -------------------
             #break
        #end
      } // end of texture
     #ifdef(Photons) 
         #if (Photons)
            photons {  // photons block
               target 1
               refraction on
               reflection on
               collect on
            }
         #end
     #end
}
#end

#macro AddYZPlate(D)
union{  // YZ plate
      #if (strlen(YZimage)>0)
         polygon {
           5,
           #if (TilYZimage)
              <0, 0>, <0, 1.1*(SceneMaxY-SceneMinY)>, <-SceneMaxZ, 1.01*(SceneMaxY-SceneMinY)>, <-SceneMaxZ, 0>, <0, 0>
           #else
              <0, 0>, <0, 1>, <1, 1>, <1, 0>, <0, 0>
           #end
           // internal texture
           #if ( __MattSet )
              texture { __MattTexture }
           #end
           texture {
             finish { ImgFinish }
             pigment {
                     #if (file_exists(YZimage))
                       ImageType( YZimage  )
                       #if (Debug)
                         #debug concat(endl, "AddYZPlate ", YZimage,": ", _FileName, ": [ext=",__ImageType, "]",endl)
                       #end
                       image_map { // exr | gif | hdr | iff | jpeg | pgm | png | ppm | sys | tga | tiff
                          #if (__ImageType="jpeg") jpeg #end
                          #if (__ImageType="tiff") tiff #end
                          #if (__ImageType= "png")  png #end
                          #if (__ImageType= "gif")  gif #end
                          #if (__ImageType= "iff")  iff #end
                          #if (__ImageType= "hdr")  hdr #end
                          #if (__ImageType= "exr")  exr #end
                          #if (__ImageType= "tga")  tga #end
                          #if (__ImageType= "pgm")  pgm #end
                          #if (__ImageType= "ppm")  ppm #end
                          #if (__ImageType= "sys")  sys #end
                          YZimage     // actual file name

                          gamma
                          #if (ImgGamma>0) ImgGamma #else srgb #end
                          #if (!TilYZimage) once #end
                          #switch (ImgInter)
                             #case (2) interpolate 2 #break // bilinear
                             #case (3) interpolate 3 #break // bicubic
                             #case (4) interpolate 4 #break // normalized
                             #else #debug "image_map: enter 2 or 4" #break
                          #end
                          #switch (ImgFilter)
                             #case (1) filter   all ImgPower #break  // filtered
                             #case (2) transmit all ImgPower #break  // transmitted
                             #else #debug "[!] - default setting for \'filter/transmit\' used. Good choice!\n" #break
                          #end
                          #if ( __ImgFilterSet )
                              filter   all __ImgFilterPower
                          #end
                          #if ( __ImgTransmitSet )
                              transmit   all __ImgTransmitPower
                          #end
                        }
                    #else
                        color rgb 0.5
                        #debug "Image specified not found"
                    #end

                }
             }
             #if (!TilYZimage)
                scale <SceneMinZ-SceneMaxZ, 1.1*(SceneMaxY-SceneMinY), 1>
             #end

             #if (camera_x>0)
                rotate  -90*y
                translate <SceneMinX-Tiny/2, SceneMinY, SceneMaxZ>
             #else
                rotate   90*y
                translate <SceneMaxX-Tiny/2, SceneMinY, SceneMinZ>
             #end
           }
      #end

// /*
      box {
         #if (camera_x>0)
           <SceneMinX-D-Tiny, -D, SceneMinZ+(camera_z>0?0:D)> ,  <SceneMinX-Tiny, 1.1*(SceneMaxY-SceneMinY), SceneMaxZ-(camera_z>0?D:0)>      // background wall Y-Z
         #else
           <SceneMaxX, -D, SceneMinZ+(camera_z<0?D:0)>,  <SceneMaxX+D, 1.1*(SceneMaxY-SceneMinY), SceneMaxZ-(camera_z<0?0:D)>
         #end
         #switch (DeclareLevel)
           #case(0)  // apparently colour only is defined
                      material{ texture { pigment{tmpColour1}
                      #if (Effects)
                         normal { bumps 0.5 scale 0.05} finish { diffuse 0.9 phong 1 }
                      #end
                   } // end of texture
                } // end of material -------------------
           #break
           #case(2) // apparently texture only is defined
                     material{ texture { tmpTexture } // end of texture
                     #if (Effects)
                       normal { bumps 0.5 scale 0.05} finish { diffuse 0.9 phong 1 }
                     #end
                     interior{ tmpInterior } // end of interior
                  } // end of material & interior -------------------
           #break
         #end
         translate <0, SceneMinY,0>
      } /// */  // end of wall definition
      #if (Photons)
         photons {  // photons block
           target 1
           refraction on
           reflection on
           collect on
         }
      #end
}
#end

// Images are to be drawn over chosen plates: done!
#macro AddAllPlates(D)
    union {
      AddXZPlate(D)

      AddXYPlate(D)

      AddYZPlate(D)

    }
#end



#end // ifdef CoordsSys_INC
/// end of INC file


