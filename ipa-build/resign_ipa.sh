if [ $# -lt 2 ]; then
	echo "Please enter '$0 ipa provision'"
	exit 1
fi
ipa=$(cd `dirname "$1"`;pwd)/`basename "$1"`
provision=$(cd "`dirname "$2"`";pwd)/`basename "$2"`
appfolder=`unzip -l $ipa | gawk '{if(match($0,/Payload\/([^\/]+\.app)\//,m)){print m[1];exit }}' |head -1`
#provision='/Users/congling/Library/MobileDevice/Provisioning Profiles/EA3508DC-257B-48A6-A549-0402177C6C98.mobileprovision'
# appfolder='totocn.app'
echo $provision
echo $appfolder


tmpfolder=/tmp/resign_ipa_`uuidgen`
mkdir $tmpfolder
cd $tmpfolder
unzip $ipa
rm -rf "Payload/$appfolder/_CodeSignature/"
cp "$provision" "Payload/$appfolder/embedded.mobileprovision"
cat "$provision" |security cms -D >p.plist
/usr/libexec/PlistBuddy -c "Print :Entitlements" p.plist  -x >Entitlements.plist
certdn=`/usr/libexec/PlistBuddy -c "Print :DeveloperCertificates:0"  p.plist |openssl x509 -inform DER -subject |gawk '{if(match($0,/\/CN=([^\/]+)\//,m)) print m[1]}'`
if !(codesign -fs "$certdn" --entitlements Entitlements.plist  "Payload/$appfolder"); then
	echo "Failed to codesign"
	rm -rf $tmpfolder
	exit 1
fi
rm $ipa
zip -r $ipa "Payload/$appfolder"
cd /tmp/
rm -rf $tmpfolder
