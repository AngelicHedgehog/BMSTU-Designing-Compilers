a='a partridge in a pear tree_two turtle doves\nand _three french hens, _four calling birds, _five gold rings;\n_six geese a-laying, _seven swans a-swimming,\n_eight maids a-milking, _nine ladies dancing, _ten lords a-leaping,\n_eleven pipers piping, _twelve drummers drumming, '.split('_')
for i in range(12):print(f"On the {'first_second_third_fourth_fifth_sixth_seventh_eighth_ninth_tenth_eleventh_twelfth'.split('_')[i]} day of Christmas my true love gave to me\n{''.join(a[i::-1])}.\n")
