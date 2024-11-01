# we are in Resources/Deployment/macOS/ -> change directory to project root
cd ../../../MenuBarMatrixMonitor

# set convenience variables
JUCEDir=../submodules/JUCE
ProjucerPath="$JUCEDir"/extras/Projucer/Builds/MacOSX
ProjucerBinPath="$ProjucerPath"/build/Release/Projucer.app/Contents/MacOS/Projucer
JucerProjectPath=MenuBarMatrixMonitor.jucer
XCodeProjectPath=Builds/MacOSX/MenuBarMatrixMonitor.xcodeproj

# build projucer
xcodebuild -project "$ProjucerPath"/Projucer.xcodeproj -configuration Release -jobs 8

# export projucer project
"$ProjucerBinPath" --resave "$JucerProjectPath"

# start building the project. The provisioning profile specification refers to a profile manually created and physically present on the build machine...
xcodebuild -project "$XCodeProjectPath" -configuration Release -jobs 8 PROVISIONING_PROFILE_SPECIFIER="296a0c41-452a-49c3-ba0e-4ae63267e7d8"
