case $1 in
R) st-flash --reset write Release/DUMRON.bin 0x8000000 ;;
D) st-flash --reset write Debug/DUMRON.bin 0x8000000 ;;
esac