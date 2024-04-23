ns_cmake_init()

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

ns_create_target_cs(LIBRARY ${PROJECT_NAME})

include(CSharpUtilities)

csharp_set_designer_cs_properties(
	Properties/AssemblyInfo.cs
	Properties/Resources.Designer.cs
	Properties/Resources.resx
	Properties/Settings.Designer.cs
	Properties/Settings.settings)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_REFERENCES
	"Microsoft.CSharp"
	"PresentationCore"
	"PresentationFramework"
	"System"
	"System.Core"
	"System.Data"
	"System.Data.DataSetExtensions"
	"System.Net.Http"
	"System.Xaml"
	"System.Xml"
	"System.Xml.Linq"
	"Microsoft.VisualStudio.Composition"
	"Microsoft.VisualStudio.Composition.Analyzers"
	"Microsoft.VisualStudio.Composition.AppHost"
	"AdonisUI"
	"CommunityToolkit.Mvvm"
	"Dirkster.AvalonDock"
	"System.ComponentModel.Composition"
	"WindowsBase")

ns_nuget_get_packages(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/packages)