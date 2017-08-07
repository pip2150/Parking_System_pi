./main
i=$?
echo $i
while [ $i -eq '1' ]; do
./main
sleep 1
done
echo $i
