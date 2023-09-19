/****h* ScPovPlot3D/BoxWhisk.inc
* PURPOSE
*   Old. well known macro for drawing 3D Box & Whiskers plot
*     image:./imgs/BoxWhiskS.jpg
*     |html <hr width=50% align="left">
*
*   Fig.[BoxWhisk] Box & whiskers plots
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

#ifndef  (BoxWhisk_INC)
#declare BoxWhisk_INC="2014-05-25";

#version 3.7;
#declare _FileName   = "BoxWhisk.inc";
#declare _FileVerMaj = "3.1.0.1";
#declare _FileVerMin = BoxWhisk_INC;
#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"
#include "metals.inc"
// various glass finishes, colors and interiors
//
#include "glass.inc"       
#include "CommonDefs.inc"

#ifndef (Debug) // Just 4 safety
   #declare Debug=0;
#end

#declare T_Glass = texture {
   pigment { color rgbft <.9,.95,.9,.8,.0> }
//   finish {
//      ambient 0.73
//      diffuse 0.7
//      reflection 0.0050
//      phong 0.2
//      phong_size 90
//   }
}


#declare Red = texture {
pigment { color rgb<0.8, 0.2, 0.2> }
finish { ambient 0.2 diffuse 0.5 }
}

#declare Green = texture {
pigment { color rgb<0.2, 0.8, 0.2> }
finish { ambient 0.2 diffuse 0.5 }
}

#declare Blue = texture {
pigment { color rgb<0.2, 0.2, 0.8> }
finish { ambient 0.2 diffuse 0.5 }
}

/********************************************/
#declare deltaX=1.0;
#declare deltaZ=1.0;

#declare BaseX=deltaX;
#declare BaseZ=deltaZ;
#declare BaseZL=deltaZ;
#declare MaxY=0.0;
#declare NRow=1;

/****f* BoxWhisk.inc/Buble()
* PURPOSE
*  Draws sphere as a hole in center of the barrel
* SYNOPSIS
Buble(X0, Y0, Z0, R)
* INPUTS
*  X0, Y0, Z0 - center of the sphere
*  R - radius
* SEE ALSO  
*  BoxAndWhisk()
*  
****/
#macro Buble(X0, Y0, Z0, R)
        sphere{
          <X0, Y0, Z0>, R
//          texture {pigment{color rgbt<0,0,0,.77>}}
                   texture { T_Copper_5A
                   //normal { bumps 0.5 scale 0.15}
                   finish { diffuse 0.8 phong 1}
                 } // end of texture

        }
#end

/****f* BoxWhisk.inc/BoxAndWhisk()
* PURPOSE
*  Draws single barrel with plates
* SYNOPSIS
BoxAndWhisk(Y0, Y1, Rad)
* INPUTS                                    
*  Barrel is created centered on z=0
*  Y0 - barrel vertical size  (ex. median, )
*  Y1 - plates position (ex. confidence range) 
*  Rad - radius of the barrel
* SEE ALSO
*  Buble()
*  AddBoxWhisker()
****/
#macro BoxAndWhisk(Y0, Y1, Rad) // center in <0,0,0> barrel and plates size, radius
      difference{
          merge{
               cylinder{<BaseX, -Y0/2, 0>, <BaseX,  Y0/2,    0>, Rad}   // cental barrel
               cylinder{<BaseX, -Y1/2, 0>, <BaseX, -Y1/2+.2, 0>, Rad}   // lower plate
               cylinder{<BaseX,  Y1/2, 0>, <BaseX,  Y1/2-.2, 0>, Rad}   // upper plate
               cylinder{<BaseX, -Y1/2, 0>, <BaseX,  Y1/2,    0>, Rad/5} // central column
               interior {
                    ior 1.5
                    fade_power 1001
                    fade_distance 0.9
                    fade_color <0.5,0.8,0.6>
               }

               photons {  // photon block for an object
                    target 1.0
                    refraction on
                    reflection on
               }

          }

      Buble(BaseX, 0, 0,min(Y0, Rad)/5)
      }
#end


/****M* BoxWhisk.inc/AddBoxWhisker()
* PURPOSE
*  Adds final barrel to the chart and sets several parameters
* SYNOPSIS
AddBoxWhisker(Y0, Y1, Mid, Rad, Tex)
* INPUTS              
*  Barrel is created centered on z=0 then moved vertically to 'Mid' value
*  Y0  - barrel vertical size  (ex. median, )
*  Y1  - plates position (ex. confidence range) 
*  Mid - value for the center (ex. mean) 
*  Rad - radius of the barrel          
*  Tex - identifier of valid texture
* SEE ALSO
*  Buble()
*  BoxWhisk()
****/
#macro AddBoxWhisker(Y0, Y1, Mid, Rad, Tex)
#declare BaseX=BaseX+Rad;
    object{
       BoxAndWhisk(Y0, Y1, Rad)
       translate<0, Mid, -BaseZ-2*Rad>
        material{ texture { Tex
                 } // end of texture
        } // end of material -------------------
    }

#if (Mid+Y1 > MaxY)
    #declare MaxY=Mid+Y1;
#end

#declare BaseX=BaseX+Rad+deltaX;
#declare BaseZL=2*deltaZ+2*Rad;

#end

/****f* BoxWhisk.inc/AddBasePlates()
* PURPOSE
*  Adds coord system stub
* SYNOPSIS
AddBasePlates(D)
* INPUTS              
*  D - width of coord system plates
* SEE ALSO
*  CoordSys.inc
****/
#macro AddBasePlates(D) // obsolete?
    #declare Corner = <BaseX, 1.01*MaxY, -NRow*BaseZL>; 
    merge{
        box{ <-D,           -D,      D>, <Corner.x,                0.0,      Corner.z> } // foundation/base
                                                                             
        box{ <-D-Tiny, -D-Tiny, D-Tiny>, <Corner.x+Tiny,      Corner.y,             0> } // background

        box{ <-D+Tiny, -D+Tiny, D+Tiny>, <            0, Corner.y+Tiny, Corner.z+Tiny> } // left side wall

        texture  { T_Copper_1A }// T_Silver_1A T_Yellow_Glass } //T_Vicksbottle_Glass } // or: Silver_Metal, T_Glass } T_Copper_4A T_Glass3 T_Winebottle_Glass   T_Copper_3A   T_Silver_1B T_Chrome_5E
     //   interior { I_Glass caustics .1 } // I_Glass_Dispersion1 | I_Glass1  I_Glass_Caustics2   
  }
#end


/****f* BoxWhisk.inc/AddPlates()
* PURPOSE
*  Adds coord system stub
* SYNOPSIS
AddBasePlates(_D, _T, _I)
* INPUTS              
*  _D - width of coord system plates
* SEE ALSO
*  CoordSys.inc
****/
#macro AddPlates(_D, _T, _I) // NEW, 2015-12-15
    #declare Corner = <BaseX, 1.01*MaxY, -NRow*BaseZL>; 
    merge{
        box{ <-_D,           -_D,      _D>, <Corner.x,                0.0,      Corner.z> } // foundation/base
                                                                             
        box{ <-_D-Tiny, -_D-Tiny, _D-Tiny>, <Corner.x+Tiny,      Corner.y,             0> } // background

        box{ <-_D+Tiny, -_D+Tiny, _D+Tiny>, <            0, Corner.y+Tiny, Corner.z+Tiny> } // left side wall

        texture  { _T }// T_Silver_1A T_Yellow_Glass } //T_Vicksbottle_Glass } // or: Silver_Metal, T_Glass } T_Copper_4A T_Glass3 T_Winebottle_Glass   T_Copper_3A   T_Silver_1B T_Chrome_5E
        interior { _I } // I_Glass_Dispersion1 | I_Glass1  I_Glass_Caustics2 { I_Glass caustics .1}  
  }
#end


#end // #ifndef
