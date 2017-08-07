./main
i=$?
while [ $i -eq '1' ]; do
./main
i=$?
sleep 2
done
echo $?
