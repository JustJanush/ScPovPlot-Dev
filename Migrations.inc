/****h* ScPovPlot3D/Migrations.inc
* PURPOSE                     
*   image:./Plot3dv3/imgs/GridnColorMap.png
*
*   Fig.[GridnColorMap] Nothing
*   |html <hr width=50% align="left"> 
*     *********************************************************
*     **   Tested on PovRay 3.7                              **
*     **   License: GNU GPL                                  **
*     **   Homepage:    http://scpovplot3d.sourceforge.net   **
*     *********************************************************
*     **   version: 3.2.0.6 (& have a nice time ;)             **
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
* HISTORY
*  2016-10-08 - Initial release
****
  end of RoboDoc comment
****/
#ifndef(Migrations_INC)
#declare Migrations_INC = "2016-10-08"; // last modification date
#version 3.7;
#local _FileVerMaj ="3.2.0.6";
#local _FileVerMin ="2016-10-08";
#local _FileName   ="Migrations.inc";                                

#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

// a lot of stony textures: T_Stone1 - T_Stone44
// Standard pre-defined colors
#include "math.inc"
#include "colors.inc"
#include "textures.inc"    
#include "stones1.inc"

#include "CommonDefs.inc"
#include "CoordsSys.inc"
#include "Cameras.inc"

/*
title           : "Journey of Matylda&Rudolf, 2016-06-22", 
num of paths    : 111,
name of path    : "Matylda"
nodes in path   : 5, #TexureNum, #LineRadius, #EndingsRadius
  timestamp   
    longitude,
     latitude, 
      elevation, 
                : 241241251,15.325222, 52.114411, 123, 
..
..
..

Next path:
name of path    : "Rudolf"
nodes in path   : 15, #TexureNum, #LineRadius, #EndingsRadius
..
..
..
*/                                             

#declare _PathTxtrs = array[6]{
   texture{pigment{rgb <.5, .5, .5>} finish{emission .6}},
   texture{pigment{rgbt <.2, .5, .1, 0>} finish{emission .6}},
   texture{pigment{rgbt <.5, .2, .1, 0>} finish{emission .76}},
   texture{pigment{rgb <.5, .2, .6>} finish{emission .76}},
   texture{pigment{rgb <.5, .3, .7>} finish{emission .76}},
   texture{pigment{rgb <.5, .7, .5>} finish{emission .76}}
}

#declare _PthLinRadius   = 0.02;

#macro ImportPaths(_filenam)
#if (file_exists(_filenam))   
   #fopen _DFile _filenam read
   #read(_DFile, _Descript) // Description       
   #read(_DFile, _NumPths, _Npts)  // number of paths, max num of nodes
   #declare PathLineTable = array[_NumPths+1][_Npts+1][4]; // [0][0][0] - num of paths, [0][*][*] - reserved
                                                           // [k][0][0] - num of points in `k`-th streamline, may depend on field's structure n<=Npts
                                                           // [x,y,z,t] - timestamp, longitud, latitud, elevation
   #declare PathLineTable[0][0][0] = _NumPths;
   #for(_Pnum, 1, _NumPths)
        #read(_DFile, _PthName, _NumNod, _R, _G, _B)
#if (Debug>1) #debug concat(_PthName,", Nodes: ", str(_NumNod,4,0), "\n") #end
        #declare PathLineTable[_Pnum][0][0] = _NumNod;
        #declare PathLineTable[_Pnum][0][1] =      _R;
        #declare PathLineTable[_Pnum][0][2] =      _G;
        #declare PathLineTable[_Pnum][0][3] =      _B;
        
        #for(_Nnum, 1, _NumNod)
            #read(_DFile, _time, _long, _lati, _elev)
            #declare PathLineTable[_Pnum][_Nnum][0] =  _time;
            #declare PathLineTable[_Pnum][_Nnum][1] =  _long;
            #declare PathLineTable[_Pnum][_Nnum][2] =  _elev;
            #declare PathLineTable[_Pnum][_Nnum][3] =  _lati-6.7; // '-' removed 9 IV
#if (Debug>1) #debug concat("Node: ",str(_Nnum,3,0),", L/L/E/T: ",str(_long,10,7),", ", str(_lati,10,7),", ", str(_elev,10,7), ", ",str(_time,10,3),"\n") #end
        #end
   #end                  
#else
   #debug concat("File ", _filenam  ,"not found!\n")
#end   
    #fclose _DFile
#end
        

/****M* VectorField.inc/DrawPathLines
* PURPOSE
*  Draws streamlines based on Streamline table _SL, spline type _splt and texture _txt. 
*  Uses _EndingPointRad internal global variable
* SYNOPSIS
*/
#macro DrawPathLines ( _SL, _splt )// - table, spline, texture
/* 
* INPUTS
*  garray     _SL   - streamlines table over N lines over N_i nodes
*  integer    _splt - interpolation type: 2 - bezier spline, 3 - cubic spline, other - linear_spline
* SIDE EFFECTS
*  Uses _EndingPointRad & _PthLinRadius internal global variables, governing radius of points.
* SEE ALSO
*  ComputeStreamline, VectorCF, DrawStreamLines
*   
******/
    #if (Debug>1) 
      #debug concat("Tot. Lines: ", str(_SL[0][0][0],5,0),"\n")           
    #end  

    #for(jnd, 1, _SL[0][0][0]) 
        #if (Debug>1) 
           #debug concat("Line: ", str(jnd,5,0),"\n")           
        #end  

        #local _np = _SL[jnd][0][0];             
        #if (_np>2)
            sphere_sweep{//
               #switch(_splt)
                 #case( 2 ) 
                    b_spline      
                 #break
                 #case( 3 ) 
                    cubic_spline  
                 #break
                 #else 
                    linear_spline
               #end  
               
               #if ((_splt=2) | (_splt=3))
                  _np+2 // increase for Bezier & cubic interpolation _SL[jnd][0][0]
                  2*<_SL[jnd][ 1 ][1], _SL[jnd][ 1 ][2], _SL[jnd][ 1 ][3]> - <_SL[jnd][ 2 ][1], _SL[jnd][ 2 ][2], _SL[jnd][ 2 ][3]>, _PthLinRadius  
               #else
                  _np
               #end
                            
               #for(ind, 1, _np)            
                  #if (Debug>1) 
                    #debug concat("Line: ", str(jnd,5,0), ", Node: ", str(ind,5,0),"\n")           
                  #end  
                  < _SL[jnd][ind][1], _SL[jnd][ind][2], _SL[jnd][ind][3]>, _PthLinRadius  //+abs(_SL[jnd][ind][1]-6)/70  
               #end              
    
               #if ((_splt=2) | (_splt=3))
                 2*<_SL[jnd][_np][1], _SL[jnd][_np][2], _SL[jnd][_np][3]> - <_SL[jnd][_np-1][1], _SL[jnd][_np-1][2], _SL[jnd][_np][3]>, _PthLinRadius  
               #end
               tolerance 1E-6
               texture {_PathTxtrs[jnd]}
               // texture{ pigment{ color rgb <_SL[jnd][0][1], _SL[jnd][0][2], _SL[jnd][0][3]>} finish{emission .6}}
            }
            #if (_EndingPointRad>0)
              sphere{ <_SL[jnd][ 1 ][1], _SL[jnd][ 1 ][2], _SL[jnd][ 1 ][3]>, _EndingPointRad texture{pigment{color rgb< 1.0, .1,  .1>} finish{Dull}}}//scDullMirror}}} // starting point                                                                  
              sphere{ <_SL[jnd][_np][1], _SL[jnd][_np][2], _SL[jnd][_np][3]>, _EndingPointRad texture{pigment{color rgb<  .1, .1, 1.0>} finish{Dull}}}//scDullMirror}}} // ending point
            #end  
         #end   
    #end
#end // #macro DrawPathLines 

/****M* VectorField.inc/MakeSpline
* PURPOSE
*  Defines spline object based on Pathline/Streamline table _SL, spline type _splt and REAL timestep
*  --
* SYNOPSIS
*/
#macro MakeSpline ( _SL, _splt, _pth )// - pathline table, spline type 1/2/3, path number, 1..N
/* 
* INPUTS
*  garray     _SL   - streamlines table over N lines over N_i nodes
*  integer    _splt - interpolation type: 2 - bezier spline, 3 - cubic spline, other - linear_spline
*  integer    _pth  - path index, 1..N
* SIDE EFFECTS
*  Nothing
* SEE ALSO
*  ComputeStreamline, VectorCF, DrawStreamLines, DrawPathLines
*   
******/
    #if (Debug>1) 
      #debug concat("Tot. Lines: ", str(_SL[0][0][0],5,0),"\n")           
    #end  

    #local jnd = _pth;
    #if (Debug>1) 
       #debug concat("Line: ", str(jnd,5,0),"\n")           
    #end  

    #local _np = _SL[jnd][0][0];             
    #local _MaxTime = _SL[jnd][_np][0];              
    #if (Debug>1) 
       #debug concat("MaxTime=", str(_MaxTime,5,0),"\n")           
    #end  

    #if (_np>2)
        spline{//
           #switch(_splt)
             #case( 2 ) 
                quadratic_spline
             #break
             #case( 3 ) 
                cubic_spline  
             #break
             #case( 4 ) 
                natural_spline
             #break
             #else 
                linear_spline
           #end  
           
           #if (_splt=3)
              // #local _np =_np+2; // increase for Bezier & cubic interpolation _SL[jnd][0][0]
              -.1, 2*<_SL[jnd][ 1 ][1], _SL[jnd][ 1 ][2], _SL[jnd][ 1 ][3]> - <_SL[jnd][ 2 ][1], _SL[jnd][ 2 ][2], _SL[jnd][ 2 ][3]>,
           #end
                        
           #for(ind, 1, _np)            
              #if (Debug>1) 
                #debug concat("Line: ", str(jnd,5,0), ", Node: ", str(ind,5,0),"time: ", str(1.0*(ind-1)/(_np-1),9,6), "\n")           
              #end  
              _SL[jnd][ind][0]/_MaxTime, < _SL[jnd][ind][1], _SL[jnd][ind][2], _SL[jnd][ind][3]>,   //+abs(_SL[jnd][ind][1]-6)/70  
           #end              

           #if (_splt=3)
             1.1, 2*<_SL[jnd][_np][1], _SL[jnd][_np][2], _SL[jnd][_np][3]> - <_SL[jnd][_np-1][1], _SL[jnd][_np-1][2], _SL[jnd][_np][3]>
           #end
        }
     #end   
#end // #macro MakeSpline

/****M* VectorField.inc/MakeSplineV
* PURPOSE
*  Defines spline object based on Pathline/Streamline table _SL, spline type _splt and synthesized, ie. Virtual timestep
*  --
* SYNOPSIS
*/
#macro MakeSplineV ( _SL, _splt, _pth )// - pathline table, spline type 1/2/3, path number, 1..N
/* 
* INPUTS
*  garray     _SL   - streamlines table over N lines over N_i nodes
*  integer    _splt - interpolation type: 2 - bezier spline, 3 - cubic spline, other - linear_spline
*  integer    _pth  - path index, 1..N
* SIDE EFFECTS
*  Nothing
* SEE ALSO
*  ComputeStreamline, VectorCF, DrawStreamLines, DrawPathLines
*   
******/
    #if (Debug>1) 
      #debug concat("Tot. Lines: ", str(_SL[0][0][0],5,0),"\n")           
    #end  

    //#for(jnd, 1, _SL[0][0][0]) 
    #local jnd = _pth;
        #if (Debug>1) 
           #debug concat("Line: ", str(jnd,5,0),"\n")           
        #end  

        #local _np = _SL[jnd][0][0];             
        #if (_np>2)
            spline{//
               #switch(_splt)
                 #case( 2 ) 
                    quadratic_spline
                 #break
                 #case( 3 ) 
                    cubic_spline  
                 #break
                 #case( 4 ) 
                    natural_spline
                 #break
                 #else 
                    linear_spline
               #end  
               
               #if (_splt=3)
                  // #local _np =_np+2; // increase for Bezier & cubic interpolation _SL[jnd][0][0]
                  -.1, 2*<_SL[jnd][ 1 ][1], _SL[jnd][ 1 ][2], _SL[jnd][ 1 ][3]> - <_SL[jnd][ 2 ][1], _SL[jnd][ 2 ][2], _SL[jnd][ 2 ][3]>,
               #end
                            
               #for(ind, 1, _np)            
                  #if (Debug>1) 
                    #debug concat("Line: ", str(jnd,5,0), ", Node: ", str(ind,5,0),"time: ", str(1.0*(ind-1)/(_np-1),9,6), "\n")           
                  #end  
                  1.0*(ind-1)/(_np-1), < _SL[jnd][ind][1], _SL[jnd][ind][2], _SL[jnd][ind][3]>,   //+abs(_SL[jnd][ind][1]-6)/70  
               #end              
    
               #if (_splt=3)
                 1.1, 2*<_SL[jnd][_np][1], _SL[jnd][_np][2], _SL[jnd][_np][3]> - <_SL[jnd][_np-1][1], _SL[jnd][_np-1][2], _SL[jnd][_np][3]>
               #end
            }
         #end   
    //#end #for
#end // #macro MakeSplineV

// Migrations.inc END  

#end