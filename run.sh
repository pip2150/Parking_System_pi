./main
i=$?
echo $i
while [ $i -eq '1' ]; do
./main
i=$?
sleep 2
echo $i
done
echo $?
