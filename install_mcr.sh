curl -o /c/MCR_installer.zip $1
unzip -q /c/MCR_installer.zip -d /c/MCR_setup

cd /c/MCR_setup
./setup -mode silent -agreeToLicense yes -destinationFolder /c/MCR -outputFile /c/MCR_log.txt

sleep 10s
if [ ! -e /c/MCR_log.txt ] ; then echo "MATLAB download doesn't start" && exit 1 ; fi
while ! tail -1 /c/MCR_log.txt | grep -q 'End'
do
sleep 10s
done

if ! tail -1 /c/MCR_log.txt | grep -q 'End - Successful' ; then echo "Fail download MCR" && exit 1 ; fi

echo "MCR is downloaded"
