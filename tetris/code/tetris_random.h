#if !defined(TETRIS_RANDOM_H)

global_variable uint32 RandomNumberTable[] ={
    201467745, 48883510, 299367538, 876983906, 59609350, 816749376, 681575521, 80045144, 781907983, 930477692,
    498697105, 698402654, 370721060, 366903605, 774645689, 949194651, 445845025, 958234683, 848879683, 129526504,
    236137034, 937497232, 50121883, 571426130, 977542666, 445155408, 596980026, 331933514, 635269081, 347972610,
    833937650, 626050164, 248679113, 550017581, 273635996, 319519366, 232430524, 718456170, 173445463, 527946241,
    902373522, 785192184, 20224687, 691276973, 406517386, 125122138, 242351518, 365996362, 335466195, 869748801,
    805346388, 635866287, 655283353, 471268472, 784718438, 75276999, 489747967, 135704, 396190995, 528713595,
    404684817, 934211628, 729820591, 148512556, 882194830, 458972259, 797498737, 964043278, 677744891, 883630139,
    568456243, 539696101, 503279845, 341891847, 492078223, 19997673, 771576955, 484235307, 109274063, 227977152,
    903733828, 641079212, 101084744, 621788222, 688779652, 504565248, 918246946, 250588306, 952600755, 843191461,
    343473671, 342782352, 592562313, 451926503, 135916569, 856704190, 14061085, 133364344, 933122364, 811364278,
    83383098, 437780644, 809009539, 691673980, 291827612, 566191601, 715229795, 216895290, 615303635, 194709432,
    315104408, 382678936, 391334285, 172783720, 962169610, 959517297, 40923584, 710873879, 837727077, 80624311,
    299564066, 307622003, 401003215, 624680655, 530679012, 567230331, 238401227, 505108447, 863165753, 492931569,
    555264863, 359963414, 587329681, 35881667, 247217184, 480836142, 429628065, 68451596, 468032429, 955680110,
    581717321, 595340630, 810997703, 571780038, 918201043, 436782248, 378317628, 206383199, 394932406, 869892521,
    904401095, 470735748, 117762656, 979212000, 755199046, 577258616, 479645397, 417440485, 198764435, 548594998,
    966088326, 561197111, 63633425, 260587644, 599774626, 625207194, 349898269, 520797664, 541771933, 711912096,
    173346937, 153725139, 330521743, 809779034, 2570946, 796487586, 981110968, 534699466, 993125055, 806942645,
    663710634, 581553664, 798109587, 670664937, 476641542, 97526617, 545960900, 925604007, 470466175, 135849722,
    764165687, 518742097, 964140970, 868171673, 100219625, 338729163, 494355444, 643786965, 760922629, 239306518,
    250798957, 128130406, 699449658, 568673684, 321283698, 58823285, 381803746, 994412239, 321442584, 164758743,
    298881055, 434257317, 491020211, 74829455, 801570870, 88669943, 223136855, 538395863, 159022949, 190651739,
    741756478, 846147369, 185655411, 681710862, 253314204, 205227886, 198889830, 662745010, 742055661, 122377543,
    153344424, 341047824, 39567068, 707516041, 586911940, 473110818, 590935548, 37986113, 594893785, 227709014,
    801607801, 723837338, 52022296, 662304813, 343370688, 622740761, 55782691, 237815824, 927449850, 345917603,
    923999077, 462411889, 18012708, 189333126, 50718441, 179638059, 403936955, 577072092, 549615502, 391529151,
    925876095, 145783596, 480122317, 357699061, 761299167, 74490797, 364012368, 819300338, 637630471, 303710501,
    768804620, 399758391, 669437216, 565724547, 748584041, 595698430, 773880114, 557507337, 845520411, 298732004,
    233955470, 246070868, 312934722, 899074537, 85494366, 361127042, 978523172, 691904260, 430154393, 835152097,
    831741443, 105992626, 7721939, 347997412, 516740090, 904792159, 156214419, 781327011, 973709661, 256275389,
    251488078, 688387108, 285869617, 509525020, 210663318, 807429017, 210627836, 463277320, 169827318, 826746234,
    959767305, 715911955, 232218904, 999318389, 959255885, 569896902, 324699838, 908907015, 131410144, 424567367,
    492212017, 673694943, 86541226, 352213979, 126275383, 76257962, 499583280, 989982671, 382219834, 214439534,
    462889715, 649815781, 954276487, 793648856, 483112247, 953482041, 646453851, 520844612, 17118664, 827834038,
    697536197, 675274399, 770374491, 130279975, 187246375, 339849558, 289122694, 234292193, 824967297, 917467334,
    36369486, 715683052, 237552065, 770007083, 530355327, 785320149, 659681385, 447140026, 163901555, 796321331,
    481687572, 203827576, 437322279, 774757391, 768646389, 304469598, 985190250, 860636919, 889814969, 95772602,
    960850411, 473238694, 844796594, 487058231, 788542039, 653512271, 32847372, 637686180, 540838586, 874088640,
    724835078, 224215853, 347068477, 705259005, 767120179, 140406689, 784534597, 34148455, 758816512, 368891147,
    627414416, 851599804, 386803878, 317966984, 17636280, 976858301, 601083996, 667907556, 814101836, 355414953,
    601106863, 43715105, 351012168, 745732678, 26467916, 660006520, 781025064, 283602829, 943385470, 189726723,
    832510023, 560216950, 617773306, 963891633, 73525991, 908332104, 17616977, 850880307, 485703833, 614976002,
    145340912, 92324162, 891668608, 975037373, 842127853, 929962255, 658525380, 300357554, 361249528, 656949442,
    588197724, 358114138, 297784870, 564678753, 563885029, 91313781, 21089138, 944927962, 244191716, 822208246,
    744457654, 597003411, 558791174, 969255615, 497962411, 213496916, 311051448, 873338208, 357811183, 588153795,
    193519477, 824555016, 427075487, 182322467, 669841570, 144225552, 169936035, 299266776, 84704923, 494736224,
    868566798, 148601661, 824638211, 80236544, 286421070, 313341449, 534739906, 267925094, 736616940, 272411135,
    777184629, 194755675, 688501877, 913349806, 635820103, 338527858, 90348769, 205624499, 939393983, 828490545,
    864831264, 767697847, 481543330, 475602803, 876402041, 817377064, 743729203, 458285208, 41669916, 899162188,
    935412731, 394971417, 773548785, 405040676, 824152400, 903064483, 919153843, 889822375, 352698529, 617012728,
    906874868, 690772483, 795923458, 392615923, 359423016, 156918621, 126679500, 495276255, 311497409, 939131465,
    233837165, 931254938, 350234602, 103481218, 174185564, 501999953, 998384040, 640693705, 541228377, 755540553,
    579524265, 218325636, 771385911, 244059962, 862198917, 25807224, 144633689, 140325821, 937395600, 210562068,
    573207192, 830987941, 737142205, 952614146, 131453523, 941732079, 208649816, 961831995, 154233432, 762130722,
    800468217, 918355760, 851308534, 651302298, 278497985, 720723069, 395090645, 835160140, 969564298, 975423936,
    742613596, 899213403, 81827722, 965269586, 562038744, 413508145, 286991772, 58052417, 622944554, 448046608,
    593951689, 460401415, 646672605, 543955727, 274349122, 59257322, 7248988, 988059352, 772488225, 750141780,
    577874622, 880075894, 900658455, 942501909, 175230007, 839850012, 915547207, 829582059, 330397586, 370745703,
    745453548, 757937223, 222277523, 416893103, 696757594, 101140729, 246789548, 804731670, 239556084, 512360682,
    190978870, 723656069, 320637639, 530849261, 707082448, 565947356, 98146504, 921230487, 729838478, 604570613,
    609880623, 871267395, 815444918, 68912606, 21762304, 394233279, 974419860, 532102725, 393077614, 821360136,
    941622602, 453903713, 631656503, 904971821, 658371692, 349908638, 619561699, 908357154, 724599038, 447379262,
    256141148, 766358993, 417409250, 341385591, 448632212, 713591972, 559766849, 110272729, 645961690, 115069588,
    657868111, 750211429, 719806473, 226924762, 799054940, 506205672, 24177233, 990366827, 560904553, 812172694,
    791430332, 204180193, 611164430, 413373477, 450007998, 114070468, 268298591, 2999557, 532134259, 472961995,
    596105155, 638028092, 840457139, 883267764, 414937666, 472814782, 882427484, 302348231, 479691339, 52035764,
    390921912, 193545027, 543926491, 478072548, 341147640, 159259277, 399117302, 185329047, 277847404, 421193527,
    613802695, 570965878, 590153569, 492709431, 107339523, 708731551, 578029899, 351991917, 250939669, 111323616,
    331281490, 9979456, 750188301, 152042739, 216068285, 38776086, 552890359, 744201755, 667901851, 568388230,
    862455827, 416916484, 106569930, 412725950, 716705275, 687060586, 389411487, 1091200, 92189906, 498199412,
    378843305, 520214892, 439587131, 16015611, 30730171, 555465810, 643337565, 890840466, 937658221, 300659254,
    609976494, 697697478, 746668082, 547674450, 412348907, 879883486, 809053646, 509651256, 916924724, 396023955,
    61635973, 903495471, 769047178, 324397854, 865306489, 511779980, 527817275, 868141905, 116826930, 223975872,
    265907468, 491257124, 189511461, 632064409, 911203347, 598059609, 292499659, 286652630, 964467187, 204983647,
    5865279, 53123554, 52986854, 668960393, 43358683, 199749773, 111706185, 12288823, 977517784, 243885703,
    532132948, 333483848, 356734899, 539283601, 571692011, 139713863, 561025294, 830303895, 163707449, 374270240,
    90444608, 536063191, 131009116, 343233721, 579337173, 89819438, 208042841, 437696684, 47958480, 59888924,
    288466619, 468553157, 182508966, 832510338, 99980415, 693021753, 409642505, 83040689, 442784496, 522152126,
    359662439, 955358307, 962893460, 269912755, 674930514, 154778638, 402655519, 366065936, 760030753, 332883909,
    604005087, 61797239, 561413673, 169173513, 903768118, 986088644, 526117575, 530026594, 575840356, 623062314,
    266828409, 280364854, 975740937, 149424490, 260171674, 39967798, 205665450, 498923889, 639880816, 751680095,
    612914672, 260902208, 708203244, 204498955, 693807438, 654111737, 79167864, 281927561, 603853309, 190137157,
    740064397, 363939401, 865777192, 678127335, 520910157, 895295978, 649415022, 835221799, 546241026, 497662430,
    259961813, 226155775, 312344240, 673675768, 325873562, 597396931, 707415735, 191936226, 52140310, 170633557,
    499581938, 281006374, 392387743, 389913722, 816243885, 730631537, 873012562, 879973198, 464982487, 787422255,
    317713059, 80047533, 601370836, 287191630, 219391095, 881962892, 67432162, 234655016, 459310220, 18866050,
    219494421, 831393378, 402643361, 28757600, 130905847, 69668021, 791320471, 567125655, 972967000, 970048642,
    406088378, 847018891, 142434815, 598570603, 538171722, 295259362, 88023212, 14108061, 597791892, 350747078,
    629063570, 678359876, 386441502, 936129821, 534333371, 938990990, 181030299, 619077300, 859160624, 589409405,
    591533731, 708388160, 490498375, 760886049, 953756988, 530027279, 294571938, 863486866, 102294694, 215906317,
    694922147, 943047998, 72937157, 120729010, 714600888, 764560840, 379101192, 44940872, 387090277, 428618731,
    55374787, 436469553, 307517396, 564940502, 230522640, 837779689, 599017573, 183208389, 91109382, 422993786,
    247738239, 616771098, 603542523, 121235102, 420617881, 630968489, 595012744, 83910975, 500203684, 848624006,
    216147639, 115767826, 648847803, 468766971, 572855977, 900557146, 418656398, 378906455, 583440512, 307306342,
    775484068, 948662475, 838376164, 789677963, 367695867, 783973807, 117344174, 802250761, 490387909, 663926686,
    947971651, 359381997, 762903012, 177075410, 996427402, 135774436, 181560499, 109308090, 71155964, 34124781,
    300709045, 303203986, 119328325, 933100356, 403341757, 838175316, 993951656, 353995285, 591532134, 383764926,
    898146869, 459938747, 554695883, 858749524, 402076712, 257226025, 511434163, 754202097, 665946955, 265194492,
    149110425, 913519149, 345363373, 101553718, 254376340, 651516493, 585264717, 427640166, 397057222, 866630987,
    439123486, 583701368, 499592953, 405484643, 709467591, 872251736, 810359945, 160409003, 178528131, 751209863
};

#define TETRIS_RANDOM_H
#endif