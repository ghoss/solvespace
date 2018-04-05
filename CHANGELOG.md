ChangeLog
=========

3.0-fd3b1013
------------
* Proof-of-concept solution for [issue #320](https://github.com/solvespace/solvespace/issues/320) (recursive calls in `bsp.cpp`may cause stack overflow when loading or exporting models with triangle meshes). For a start, recursion has been substituted by a queue solution in InsertConvex(). Similar solutions could eventually be applied to the other recursive BSP functions as well.
* Some minor fixes to German translations

3.0-a49a7384
------------
* German localization
* All hard-coded English strings still present in the original branch have been localized
* Included bugfix for [issue #316](https://github.com/solvespace/solvespace/issues/316) in original repository (crash at startup under Mac OS X 10.9)
