# WDFoundation

WDFoundation is our Starting point for any C++ Project that you may need.

It's Used internally at WDStudios, Mainly our Eclipse Engine, Swift Script, and some In-House tools.

Its a battle-tested solution, being used on Playstation 5, Xbox Series X/S, And The Switch.

## Contents
Code/Foundation: Many C++ Classes to assist with any application.

Code/BuildSystem: CMake Based Build System, borrowed from the ezEngine Team, and now heavily modified.

Code/PlatformSubModules: This is where all NDA Specific Code is held, but for this public release, this had to be removed.

***This can be easily modified to add any platform to WDFoundation.***

Code/T3: C# Application for creating interactive toolsets with WPF. This is what we use for our internal tools.

Code/ThirdParty: ThirdParty Code.

Code/UnitTests: UnitTests.

Data: This is left over from our Eclipse Codebase, but the **Data** Folder contains assets needed for UnitTests. 

### Setup
Setup is very simple, just run the bat for the platform you want to develop with and code.

Make sure you extract the Precompiled Zip file to: Data\Tools\Precompiled before running anything.

### License

WDFoundation is released under the MIT-License.

### Thanks

ezEngine-Team: Thank You for giving us a framework to essentially build our entire studio upon!