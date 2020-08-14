<distribution version="17.0.0" name="HostSW" type="MSI">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{01D0D9AB-F98C-4B19-A276-84D8A59EF97A}">
		<general appName="HostSW" outputLocation="c:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvidistkit.HostSW" relOutputLocation="cvidistkit.HostSW" outputLocationWithVars="c:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvidistkit.%name" relOutputLocationWithVars="cvidistkit.%name" upgradeBehavior="1" autoIncrement="true" version="1.0.4">
			<arp company="" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="Chinese (Simplified)" showPaths="true" showRuntimeOnly="true" readMe="" license="">
			<dlgstrings welcomeTitle="HostSW" welcomeText=""/></userinterface>
		<dirs appDirID="100">
			<installDir name="[Program Files]" dirID="2" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="HostSW" dirID="100" parentID="2" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="HostSW" dirID="101" parentID="7" isMSIDir="false" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\Log_V1.05_120302.exe" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\exceldem.xlsx" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="2" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\config.txt.bak" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="3" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvirte.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="4" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvi_lvrt.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="5" sourcePath="c:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvibuild.HostSW\Release\HostSW.exe" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="6" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\TxtToExcel.uir" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="7" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\Testitem.log.bak" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="8" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvirte\bin\cvirte.rsc" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="9" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cviauto.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="10" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\ninetv.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="11" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvirte\bin\msgrt4.txt" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="12" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvirte\fonts\nisystem.ttf" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="13" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvirte\bin\msgrte.txt" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="14" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvidbgi.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="15" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvirtsup.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="16" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvirte\bin\cvirt4.rsc" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="17" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cviogl.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="18" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\nicont.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="19" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\NiReports.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="20" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\config.txt" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="21" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\nianlys.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="22" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\exceldem.xls" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="23" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cviUSI.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="24" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvitdms.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="25" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\Testitem.log" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="26" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\tdms.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="27" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvirte\bin\English\cvierror.txt" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="28" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\nicontdt.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="29" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvirt.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="30" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvidotnetv1.1.4322.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="31" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvintwrk.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="32" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\cvirte\fonts\ni7seg.ttf" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="33" sourcePath="C:\Users\Cliff\Documents\National Instruments\CVI\HostSW\cvi2009RTE\CVI2009 SP1(绿色版文件库)\mesa.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput targetType="0" dirID="100" projectID="0">
				<fileID>5</fileID></projectOutput></fileGroups>
		<shortcuts>
			<shortcut name="HostSW" targetFileID="5" destDirID="101" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products>
			<product name="NI LabWindows/CVI Shared Runtime 2019" UC="{80D3D303-75B9-4607-9312-E5FC68E5BFD2}" productID="{2EB63900-C920-494E-9F7B-56E6B2DCBEE9}" path="C:\ProgramData\National Instruments\MDF\ProductCache\" flavorID="_full_" flavorName="Full" verRestr="false" coreVer="19.0.49152">
				<cutSoftDeps>
					<softDep name=".NET Support" UC="{0DDB211A-941B-4125-9518-E81E10409F2E}" depKey="ni-cvi-dotnet.CVI1900.RTE"/>
					<softDep name="Analysis Support" UC="{86208B51-159E-4F6F-9C62-0D5EFC9324D8}" depKey="ni-cvi-analysis.CVI1900.RTE"/>
					<softDep name="Network Streams Support" UC="{40A5AD7F-4BAF-4A5C-8B56-426B84F75C05}" depKey="ni-cvi-networkstreams.CVI1900.RTE"/>
					<softDep name="Network Variable Support" UC="{15CE39FE-1354-484D-B8CA-459077449FB3}" depKey="ni-cvi-network-variable.CVI1900.RTE"/>
					<softDep name="TDMS Support" UC="{5A8AF88D-486D-4E30-A7A5-8D8A039BBEBF}" depKey="ni-cvi-tdms.CVI1900.RTE"/></cutSoftDeps></product></products>
		<nonAutoSelectProducts>
			<productID>{2EB63900-C920-494E-9F7B-56E6B2DCBEE9}</productID></nonAutoSelectProducts>
		<runtimeEngine installToAppDir="false" activeXsup="false" analysis="false" cvirte="true" dotnetsup="false" instrsup="true" lowlevelsup="false" lvrt="true" netvarsup="false" rtutilsup="false">
			<hasSoftDeps/>
			<doNotAutoSelect>
			<component>activeXsup</component>
			<component>instrsup</component>
			<component>lowlevelsup</component>
			<component>lvrt</component>
			<component>rtutilsup</component>
			</doNotAutoSelect></runtimeEngine><sxsRuntimeEngine>
			<selected>false</selected>
			<doNotAutoSelect>false</doNotAutoSelect></sxsRuntimeEngine>
		<advanced mediaSize="650">
			<launchConditions>
				<condition>MINOS_WIN7_SP1</condition>
			</launchConditions>
			<includeConfigProducts>true</includeConfigProducts>
			<maxImportVisible>silent</maxImportVisible>
			<maxImportMode>merge</maxImportMode>
			<custMsgFlag>false</custMsgFlag>
			<custMsgPath>msgrte.txt</custMsgPath>
			<signExe>false</signExe>
			<certificate></certificate>
			<signTimeURL></signTimeURL>
			<signDescURL></signDescURL></advanced>
		<baselineProducts>
			<product name="NI LabWindows/CVI Shared Runtime 2019" UC="{80D3D303-75B9-4607-9312-E5FC68E5BFD2}" productID="{2EB63900-C920-494E-9F7B-56E6B2DCBEE9}" path="(None)" flavorID="_full_" flavorName="Full" verRestr="false" coreVer="19.0.49152">
				<dependencies>
					<productID>{0946CDFA-C3C6-4AD5-B884-FB72701ED351}</productID>
					<productID>{14B23AC5-B7EF-47D1-A57D-8666BAEE13C3}</productID>
					<productID>{16BC0547-DF57-48CF-8CB5-1CC0DF3B4911}</productID>
					<productID>{52981014-740C-430E-A83A-711186DF565B}</productID>
					<productID>{5B4D362F-8203-490C-82D1-5F607972196F}</productID>
					<productID>{60684600-163F-45D7-83DB-E247FA48D81F}</productID>
					<productID>{64ECB814-3A6A-4E48-9D2F-D6C2EDD725B7}</productID>
					<productID>{69181DFA-D7CA-4AFE-98E2-A985F32B557B}</productID>
					<productID>{6E1BCD5B-41A1-4E69-8C6E-389F57A19F00}</productID>
					<productID>{75191165-D39E-42A2-A7A2-D74AE99F8A84}</productID>
					<productID>{7EE28BBA-8A24-40B1-B0D0-9066A8CFA4AA}</productID>
					<productID>{81710CDC-7B4D-4A91-8D44-8E2F8B5C2B6E}</productID>
					<productID>{820FB6E8-8856-48D7-934C-76169DCCFD48}</productID>
					<productID>{88104A0D-1729-4A73-B798-80BEFC243F18}</productID>
					<productID>{88DFF01E-7C87-4E35-B194-CB7A2E32FA6D}</productID>
					<productID>{93977567-FFEC-453C-A47F-CE30077E9F4B}</productID>
					<productID>{9B14BAA8-31A5-463B-88C8-EC81F67BEE57}</productID>
					<productID>{AFC999BB-F270-46EF-B748-AE755EC75322}</productID>
					<productID>{B0049D5A-380E-4219-A69D-02273F86891C}</productID>
					<productID>{B29007A1-E053-46E4-8A51-6803638AF06D}</productID>
					<productID>{C0A9F5E2-DCD5-44C1-8B03-C560F4C06D6C}</productID>
					<productID>{CE61F080-FB64-4F6C-9763-A3060A0E59E6}</productID>
					<productID>{D1027BA0-C959-44E2-B4FA-10386404AF75}</productID>
					<productID>{D72C00A8-14F3-4E45-AFAC-5A71F833E210}</productID>
					<productID>{E12F09FF-07B3-4677-8D5C-BDD01E9A5545}</productID>
					<productID>{ED664E5A-6D28-48EA-A2EE-B6356EF01B37}</productID>
					<productID>{EEA5281C-7DA9-492E-8F6F-4127DC67AAD6}</productID>
					<productID>{EFC648C5-F3BC-4096-9AFE-23121EF06828}</productID>
					<productID>{F2EEE63A-0DCA-41BF-A243-4E4C0DFA38A4}</productID>
					<productID>{F456DB40-F5DF-45FE-A582-83DBED5D6432}</productID>
					<productID>{F8018104-6294-4896-A9E2-CBC919992310}</productID>
					<productID>{FA85DBC5-063E-4CA0-9E3A-7C1180D8F7AE}</productID>
					<productID>{FAFB3CD7-F8A6-4D31-BCB4-57354C17128E}</productID>
					<productID>{FB67912C-BB8D-4FE2-B64E-7712EB90DEA4}</productID></dependencies></product></baselineProducts>
		<Projects NumProjects="1">
			<Project000 ProjectID="0" ProjectAbsolutePath="c:\Users\Cliff\Documents\National Instruments\CVI\HostSW\HostSW.prj" ProjectRelativePath="HostSW.prj"/></Projects>
		<buildData progressBarRate="2.597465096798153">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.978724874999902</ProductsAdded>
				<DPConfigured>1.413550187499797</DPConfigured>
				<DPMergeModulesAdded>4.336678749999947</DPMergeModulesAdded>
				<DPClosed>7.733558812499857</DPClosed>
				<DistributionsCopied>8.592219499999915</DistributionsCopied>
				<End>38.499073625000065</End></progressTimes></buildData>
	</msi>
</distribution>
