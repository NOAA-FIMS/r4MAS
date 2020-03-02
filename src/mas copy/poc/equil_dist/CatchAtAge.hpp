/*
 * File:   main.cpp
 * Author: z
 *
 * Created on May 10, 2017
 */

#ifndef CATCHATAGE_HPP
#    define CATCHATAGE_HPP

#include "../ATL2/ATL.hpp"

// #include <Eigen/Dense>
// using namespace Eigen;

#define SQUARE(x) ((x)*(x))

template<typename T>
class CatchAtAge : public atl::ObjectiveFunction<T>
{
    int npops  = 1;
    int nareas = 2;

    int nages = 10;
    atl::Vector<T> ages = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    atl::Variable<T> sigmaR = 0.5;

    int nyrs = 30;
    int nfshs = 2;
    atl::Vector<T> obs_catch_fsh1 = { 15966.5  14680.3 13658.5 13098.8 12677.8 12324.4 12064.9 11871.0 11717.8 11600.4 40366.2 31853.3 27911.0 25791.3 24389.2 23332.1 22594.9 22054.1 21679.1 21393.3 11509.5 12982.5 13744.2 14172.3 14495.6 14777.0 14985.4 15152.7 15271.6 15366.9 };
    atl::Vector<T> obs_catch_fsh2 = { 9686.4  10624.8 11055.1 11154.2 11109.3 10982.0 10841.2 10713.5 10605.5 10512.7 19998.9 18412.2 16791.4 15607.6 14790.0 14183.0 13714.1 13367.6 13109.4 12921.6 12781.2 12676.9 12846.6 13047.8 13217.4 13378.7 13532.3 13656.9 13759.8 13838.8 };
    atl::Vector<T> obs_catch_CV_fsh1 = { 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 };
    atl::Vector<T> obs_catch_CV_fsh2 = { 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 };

    int nyrs_fsh_prop_at_age = 30;
    atl::Vector<int> yrs_fsh_prop_at_age = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
    atl::Vector<T> obs_fsh1_prop_at_age_N = { 50.000   50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000  50.000 };
    atl::Vector<T> obs_fsh2_prop_at_age_N = { 25.000   25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000  25.000 };

    atl::Matrix<T> obs_fsh1_prop_at_age = {
        // 0    1    2    3    4    5    6    7   8   9
0.000000    0.000000    0.000000    0.118606    0.192065    0.144603    0.116229    0.091368    0.069366    0.267763
0.000000    0.000000    0.000000    0.125211    0.194226    0.149093    0.117321    0.089089    0.070146    0.254913
0.000000    0.000000    0.000000    0.133419    0.199955    0.153080    0.114602    0.089416    0.069536    0.239991
0.000000    0.000000    0.000000    0.135786    0.209509    0.154627    0.114600    0.087836    0.067600    0.230041
0.000000    0.000000    0.000000    0.133716    0.213428    0.158928    0.117812    0.085614    0.068039    0.222463
0.000000    0.000000    0.000000    0.137783    0.209733    0.160439    0.118066    0.089605    0.065629    0.218745
0.000000    0.000000    0.000000    0.140011    0.213213    0.158829    0.119289    0.089527    0.066022    0.213109
0.000000    0.000000    0.000000    0.141789    0.213661    0.157159    0.118849    0.090622    0.065578    0.212343
0.000000    0.000000    0.000000    0.142981    0.212796    0.162930    0.116217    0.087939    0.067947    0.209190
0.000000    0.000000    0.000000    0.145869    0.218797    0.160810    0.120982    0.087553    0.064970    0.201019
0.000000    0.000000    0.000000    0.154978    0.214719    0.160417    0.119023    0.088800    0.063017    0.199046
0.000000    0.000000    0.000000    0.190884    0.233437    0.145153    0.112010    0.079668    0.061521    0.177327
0.000000    0.000000    0.000000    0.214642    0.265619    0.150514    0.093002    0.071701    0.049112    0.155410
0.000000    0.000000    0.000000    0.227004    0.281198    0.161428    0.095961    0.058840    0.046545    0.129025
0.000000    0.000000    0.000000    0.233076    0.293018    0.166859    0.097984    0.060192    0.035250    0.113621
0.000000    0.000000    0.000000    0.240141    0.298371    0.172026    0.099488    0.057860    0.039076    0.093038
0.000000    0.000000    0.000000    0.241589    0.304599    0.173190    0.102271    0.060730    0.033751    0.083869
0.000000    0.000000    0.000000    0.247118    0.299707    0.175390    0.102232    0.063578    0.039469    0.072506
0.000000    0.000000    0.000000    0.246241    0.305836    0.175404    0.104235    0.061677    0.036488    0.070118
0.000000    0.000000    0.000000    0.252312    0.307358    0.176057    0.099600    0.062089    0.037398    0.065186
0.000000    0.000000    0.000000    0.242153    0.314062    0.177438    0.104951    0.059069    0.038212    0.064114
0.000000    0.000000    0.000000    0.221230    0.303251    0.193942    0.110061    0.066739    0.038471    0.066306
0.000000    0.000000    0.000000    0.205051    0.282884    0.198670    0.126522    0.070915    0.045543    0.070415
0.000000    0.000000    0.000000    0.199612    0.281700    0.186675    0.125289    0.083007    0.045737    0.077979
0.000000    0.000000    0.000000    0.200318    0.273859    0.184478    0.122685    0.080698    0.055701    0.082261
0.000000    0.000000    0.000000    0.196042    0.279618    0.178192    0.119853    0.082492    0.050995    0.092808
0.000000    0.000000    0.000000    0.199318    0.270728    0.184507    0.116873    0.079827    0.055594    0.093152
0.000000    0.000000    0.000000    0.194096    0.275774    0.182493    0.119637    0.079382    0.050601    0.098017
0.000000    0.000000    0.000000    0.195411    0.272942    0.180348    0.117848    0.080528    0.051978    0.100945
0.000000    0.000000    0.000000    0.195127    0.269652    0.178480    0.120997    0.079277    0.053282    0.103186
    };

    atl::Matrix<T> obs_fsh2_prop_at_age = {
        // 0    1    2    3    4    5    6    7   8   9

0.001856    0.008694    0.043111    0.139393    0.188865    0.158089    0.117553    0.087507    0.066302    0.188629
0.001637    0.008151    0.040465    0.134169    0.175572    0.152779    0.115660    0.087340    0.067221    0.217007
0.001582    0.007574    0.039942    0.131632    0.179325    0.149371    0.114576    0.087448    0.066623    0.221926
0.001571    0.007518    0.038014    0.134022    0.178030    0.149138    0.114890    0.086833    0.066736    0.223250
0.001590    0.007602    0.038473    0.128210    0.183969    0.152572    0.116066    0.085545    0.064957    0.221017
0.001590    0.007610    0.038411    0.129178    0.180500    0.156899    0.117589    0.086038    0.066504    0.215680
0.001628    0.007786    0.039391    0.129334    0.180550    0.150526    0.119877    0.089377    0.065944    0.215585
0.001635    0.007817    0.039472    0.132275    0.181556    0.152660    0.111748    0.091334    0.068049    0.213453
0.001653    0.007897    0.039891    0.134661    0.180775    0.154225    0.116868    0.087368    0.068071    0.208592
0.001657    0.007914    0.039939    0.133109    0.182419    0.154734    0.114146    0.090776    0.065081    0.210225
0.001747    0.008331    0.041836    0.137073    0.179651    0.153849    0.119631    0.085343    0.065989    0.206550
0.001858    0.008997    0.045076    0.144220    0.187679    0.153515    0.113451    0.087327    0.062462    0.195414
0.001986    0.009592    0.048791    0.159670    0.197108    0.149277    0.110604    0.078946    0.063659    0.180368
0.002098    0.010057    0.051269    0.167736    0.215205    0.156654    0.102526    0.077562    0.052522    0.164372
0.002201    0.010518    0.052982    0.174098    0.221762    0.168402    0.107284    0.067811    0.051343    0.143599
0.002253    0.010738    0.054048    0.174271    0.226592    0.170639    0.114368    0.074041    0.042631    0.130419
0.002314    0.011015    0.055225    0.180879    0.224291    0.176446    0.115084    0.073259    0.050142    0.111345
0.002351    0.011178    0.056026    0.181577    0.236041    0.170875    0.116886    0.074854    0.045861    0.104350
0.002401    0.011404    0.057058    0.186714    0.232328    0.172832    0.114096    0.075795    0.050761    0.096610
0.002434    0.011551    0.057770    0.184946    0.234291    0.172888    0.119166    0.074791    0.048251    0.093912
0.002450    0.011622    0.058064    0.184920    0.236991    0.176806    0.114484    0.076457    0.048426    0.089780
0.002473    0.011646    0.058160    0.190996    0.236097    0.174473    0.117586    0.072624    0.048666    0.087280
0.002453    0.011558    0.057302    0.186277    0.237958    0.180247    0.114341    0.076567    0.046062    0.087235
0.002426    0.011492    0.056803    0.182742    0.230981    0.180547    0.122752    0.076150    0.050388    0.085718
0.002404    0.011398    0.056835    0.184129    0.230100    0.170890    0.121565    0.082769    0.048279    0.091632
0.002371    0.011251    0.056028    0.182656    0.229937    0.175923    0.117669    0.077722    0.056372    0.090071
0.002379    0.011294    0.056381    0.179121    0.229678    0.173872    0.118558    0.079023    0.051202    0.098492
0.002337    0.011100    0.055386    0.182684    0.226058    0.173648    0.116154    0.079130    0.053326    0.100175
0.002342    0.011123    0.055559    0.177144    0.230252    0.171880    0.118706    0.077849    0.051587    0.103558
0.002313    0.010994    0.054903    0.181025    0.224564    0.171835    0.116983    0.080660    0.052034    0.104689

    };


    int nyrs_srv = 30;
    atl::Vector<int> yrs_srv = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
    atl::Vector<T> obs_srv1_biomass = { 154.4    140.5   131.1   125.8   121.8   118.5   116.1   114.2   112.8   111.7   90.2    72.1    63.7    59.2    56.1    53.8    52.1    51.0    50.1    49.5    55.7    62.4    65.8    67.8    69.3    70.6    71.5    72.3    72.8    73.3 };
    atl::Vector<T> obs_srv2_biomass = { 94.3    103.1   107.2   108.2   107.8   106.4   105.1   103.9   102.9   102.0   94.7    87.4    80.0    74.7    70.9    68.1    65.9    64.3    63.1    62.3    61.6    61.1    61.9    62.8    63.6    64.4    65.1    65.7    66.1    66.5 };
    atl::Vector<T> obs_srv1_CV = { 0.3   0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 };
    atl::Vector<T> obs_srv2_CV = { 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 };

    int nyrs_srv_prop_at_age = 30;
    atl::Vector<int> yrs_srv_prop_at_age = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
    atl::Vector<T> obs_srv1_prop_at_age_N = { 10.000 10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000 };
    atl::Vector<T> obs_srv2_prop_at_age_N = { 10.000 10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000  10.000 };
    atl::Matrix<T> obs_srv1_prop_at_age = {
0.001095    0.005412    0.028057    0.129184    0.171271    0.141231    0.109671    0.086044    0.067571    0.260463
0.001201    0.005929    0.030389    0.136539    0.175893    0.142400    0.112276    0.084544    0.066175    0.244654
0.001282    0.006240    0.033415    0.143590    0.182990    0.143414    0.109129    0.084362    0.066537    0.229041
0.001333    0.006521    0.033430    0.146924    0.190819    0.142150    0.112157    0.083919    0.063316    0.219430
0.001378    0.006813    0.034346    0.147325    0.193023    0.148279    0.111243    0.082941    0.063813    0.210839
0.001378    0.006809    0.034859    0.150732    0.190265    0.148512    0.114284    0.084564    0.062437    0.206159
0.001426    0.006926    0.034651    0.151170    0.193219    0.148239    0.115431    0.083972    0.064049    0.200917
0.001443    0.007043    0.036685    0.149871    0.195705    0.147914    0.112306    0.085151    0.063533    0.200350
0.001437    0.007007    0.036335    0.155918    0.196455    0.150047    0.111473    0.084288    0.063329    0.193711
0.001461    0.007104    0.036852    0.158444    0.199541    0.149035    0.111881    0.081502    0.062130    0.192050
0.001785    0.008553    0.044336    0.170454    0.193198    0.147981    0.108212    0.081884    0.060759    0.182838
0.002096    0.010511    0.054293    0.207739    0.205748    0.130504    0.098760    0.071082    0.056585    0.162682
0.002291    0.011494    0.059867    0.227152    0.230672    0.132665    0.083596    0.066068    0.044856    0.141339
0.002426    0.012033    0.062161    0.247696    0.235891    0.143612    0.085900    0.051939    0.042386    0.115956
0.002519    0.012400    0.064974    0.248080    0.252898    0.146497    0.085564    0.054595    0.031754    0.100720
0.002563    0.012784    0.065827    0.257137    0.247930    0.153047    0.090505    0.050996    0.035809    0.083402
0.002642    0.012907    0.067913    0.259363    0.254345    0.150626    0.093079    0.055258    0.029881    0.073985
0.002662    0.013148    0.069708    0.257658    0.253336    0.157333    0.090602    0.055766    0.034382    0.065404
0.002701    0.013039    0.069254    0.262892    0.259678    0.150248    0.092371    0.053913    0.032829    0.063074
0.002754    0.013200    0.068283    0.261457    0.262767    0.157088    0.088772    0.054737    0.033031    0.057911
0.002413    0.012088    0.061649    0.253419    0.269450    0.160415    0.094101    0.053677    0.033774    0.059013
0.002241    0.011065    0.056801    0.226644    0.267195    0.177994    0.101700    0.060337    0.035094    0.060928
0.002216    0.010640    0.054107    0.216111    0.251892    0.177159    0.115599    0.065475    0.042415    0.064386
0.002132    0.010770    0.053571    0.211020    0.245043    0.171324    0.114951    0.076317    0.043124    0.071748
0.002120    0.010117    0.051272    0.218452    0.238534    0.166505    0.113655    0.073640    0.051080    0.074626
0.002068    0.010069    0.050745    0.209724    0.245774    0.164830    0.110718    0.074476    0.047970    0.083627
0.002082    0.010104    0.050998    0.209932    0.237140    0.171590    0.108133    0.074287    0.050333    0.085401
0.002028    0.009700    0.051143    0.208822    0.245809    0.160231    0.111868    0.072205    0.046955    0.091239
0.002022    0.009975    0.050283    0.208795    0.235983    0.167202    0.108824    0.074708    0.048722    0.093487
0.002002    0.009585    0.049943    0.206943    0.242638    0.163076    0.110375    0.073056    0.049130    0.093254
    };

    atl::Matrix<T> obs_srv2_prop_at_age = {
0.001744    0.008417    0.041029    0.156255    0.198068    0.153498    0.114087    0.085075    0.062120    0.179707
0.001572    0.007683    0.038219    0.146689    0.187594    0.149317    0.113974    0.086005    0.063796    0.205153
0.001507    0.007466    0.037203    0.147614    0.183553    0.145953    0.112170    0.085135    0.065169    0.214229
0.001521    0.007407    0.036248    0.147698    0.186259    0.147073    0.110760    0.084236    0.063738    0.215061
0.001497    0.007298    0.036749    0.141657    0.190329    0.150576    0.109082    0.083302    0.063090    0.216419
0.001526    0.007312    0.036651    0.143706    0.185119    0.153748    0.113106    0.083849    0.063917    0.211067
0.001538    0.007488    0.036820    0.146387    0.187194    0.148998    0.116834    0.086195    0.063288    0.205258
0.001557    0.007555    0.036667    0.148340    0.185496    0.147051    0.113122    0.087853    0.064823    0.207535
0.001577    0.007591    0.038217    0.149126    0.190198    0.146507    0.113216    0.083938    0.066480    0.203148
0.001590    0.007735    0.038143    0.149694    0.185892    0.151891    0.111435    0.086634    0.062978    0.204008
0.001702    0.008235    0.040726    0.153117    0.190170    0.148169    0.115946    0.082457    0.064005    0.195474
0.001800    0.008829    0.043298    0.163234    0.194438    0.149323    0.108248    0.083617    0.058798    0.188414
0.001945    0.009421    0.047199    0.179155    0.205326    0.142132    0.106259    0.074874    0.060473    0.173216
0.002034    0.009999    0.049878    0.191100    0.219616    0.150545    0.098106    0.074089    0.049626    0.155008
0.002130    0.010303    0.050518    0.195271    0.227546    0.158350    0.102378    0.065008    0.050007    0.138489
0.002151    0.010515    0.052142    0.197142    0.235987    0.162290    0.106557    0.069259    0.040970    0.122985
0.002251    0.010716    0.054004    0.200355    0.233997    0.166759    0.110572    0.070419    0.046729    0.104199
0.002263    0.011057    0.053348    0.204526    0.238534    0.166115    0.109367    0.072738    0.043964    0.098090
0.002244    0.011142    0.054520    0.205615    0.241925    0.168965    0.106502    0.071320    0.047379    0.090389
0.002331    0.011067    0.054887    0.208794    0.240470    0.165781    0.111348    0.070198    0.045876    0.089249
0.002287    0.011184    0.055431    0.208927    0.242653    0.169100    0.109204    0.072451    0.044648    0.084116
0.002351    0.011248    0.056043    0.211753    0.241029    0.166428    0.111866    0.068595    0.047072    0.083614
0.002368    0.011514    0.054120    0.207411    0.244103    0.170571    0.109868    0.074108    0.043740    0.082198
0.002343    0.011107    0.053612    0.202068    0.239075    0.172653    0.117394    0.070885    0.049307    0.081555
0.002302    0.011012    0.053453    0.203453    0.237014    0.168158    0.115990    0.078763    0.045471    0.084384
0.002329    0.011031    0.054170    0.202713    0.235374    0.164479    0.112769    0.076130    0.052923    0.088081
0.002294    0.010957    0.053487    0.196933    0.240718    0.165762    0.112232    0.074425    0.048929    0.094264
0.002266    0.010759    0.053720    0.198349    0.236517    0.167286    0.110115    0.074884    0.051367    0.094738
0.002196    0.010720    0.053117    0.197086    0.236947    0.165795    0.113103    0.074627    0.049694    0.096716
0.002188    0.010561    0.053062    0.198046    0.234042    0.167238    0.110187    0.077761    0.049361    0.097554

    };

    int nsrvs = 2;

    atl::Variable<T> yrfrac_sp  = 0.25; // fraction of the year for spawning
    atl::Variable<T> yrfrac_fsh = 0.5;  // when catch wt-at-age is calculated
    atl::Variable<T> yrfrac_srv = 0.75; // when the survey occurs

    atl::Vector<T> M = { 0.5 0.3 0.2 0.2 0.2 0.2 0.2 0.2 0.2 0.2 };


   // Estimated (read only)
    atl::Matrix<atl::Variable<T> > init_pop_devs;   // area x age
    atl::Matrix<atl::Variable<T> > initial_population;  // area x age

    atl::Vector<atl::Variable<T> > log_R0;  // area
    atl::Vector<atl::Variable<T> > R0;      // area
    atl::Vector<atl::Variable<T> > log_h;   // area
    atl::Vector<atl::Variable<T> > h;       // area
    atl::Matrix<atl::Variable<T> > recruit_devs;    // area x year
    atl::Matrix<atl::Variable<T> > recruits;        // area x year

    atl::Vector<atl::Variable<T> > SB0;     // area

    atl::Matrix<atl::Variable<T>  > recr_move = { { 0.9, 0.1 },
                                                  { 0.1, 0.9 } };
    atl::Matrix<atl::Variable<T>  > pop_move  = { { 0.9, 0.1 },
                                                  { 0.1, 0.9 } };

    atl::Variable<T> log_mat_alpha = atl::log(3.0);
    atl::Variable<T> log_mat_beta  = atl::log(0.1);
    atl::Variable<T> mat_alpha;
    atl::Variable<T> mat_beta;

    atl::Vector<atl::Variable<T> > maturity;    // age

    // modified von Bertalanffy growth model
    int Amin = 1;
    int Amax = 7;
    atl::Variable<T> L_Amin = atl::Variable<T>(20.0);
    atl::Variable<T> L_Amax = atl::Variable<T>(70.0);
    atl::Variable<T> c      = atl::Variable<T>(0.5);
    atl::Variable<T> wtAA_A = atl::Variable<T>(0.000025);
    atl::Variable<T> wtAA_B = atl::Variable<T>(3.0);

    // fishing mortality
    atl::Vector<atl::Variable<T> > log_mean_fsh_mort;   // fsh (area)
    atl::Matrix<atl::Variable<T> > fsh_mort_devs;       // fsh (area) x year
    atl::Matrix<atl::Variable<T> > fsh_mort;            // fsh (area) x year

    // catchability for srvs
    atl::Vector<atl::Variable<T> > log_srv_q;           // srv (area)
    atl::Vector<atl::Variable<T> > srv_q;               // srv (area)

    //selectivity model
    atl::Vector<atl::Variable<T> > log_fsh_sel_alpha = { atl::log(3.0), atl::log(3.0) };
    atl::Vector<atl::Variable<T> > log_fsh_sel_beta  = { atl::log(0.1), atl::log(0.5) };
    atl::Vector<atl::Variable<T> > fsh_sel_alpha;
    atl::Vector<atl::Variable<T> > fsh_sel_beta;

    atl::Matrix<atl::Variable<T> > fsh_sel;             // fsh (area) x age

    atl::Vector<atl::Variable<T> > log_srv_sel_alpha = { atl::log(3.0), atl::log(2.75) };
    atl::Vector<atl::Variable<T> > log_srv_sel_beta  = { atl::log(0.25), atl::log(0.5) };
    atl::Vector<atl::Variable<T> > srv_sel_alpha;
    atl::Vector<atl::Variable<T> > srv_sel_beta;

    atl::Matrix<atl::Variable<T> > srv_sel;             // srv (area) x age


    //Runtime (writable)
    std::vector<atl::VariableMatrix<T> > N(nareas, atl::VariableMatrix<T> (nyears, nages));
    std::vector<atl::VariableMatrix<T> > F(nareas, atl::VariableMatrix<T> (nyears, nages));
    std::vector<atl::VariableMatrix<T> > C(nareas, atl::VariableMatrix<T> (nyears, nages));
    std::vector<atl::VariableMatrix<T> > Z(nareas, atl::VariableMatrix<T> (nyears, nages));
    std::vector<atl::VariableMatrix<T> > expZ(nareas, atl::VariableMatrix<T> (nyears, nages));
    std::vector<atl::VariableMatrix<T> > expZsp(nareas, atl::VariableMatrix<T> (nyears, nages));
    std::vector<atl::VariableMatrix<T> > expZsrv(nareas, atl::VariableMatrix<T> (nyears, nages));

    atl::Vector<atl::Variable<T> > wt_spawn;    // weight-at-age at the time of peak spawning
    atl::Vector<atl::Variable<T> > wt_fsh;      // weight-at-age at the time of peak fishing
    atl::Vector<atl::Variable<T> > wt_srv;      // weight-at-age at the time of the surveys
    atl::Vector<atl::Variable<T> > wt_pop;      // weight-at-age at the beginning of the year

    atl::Vector<atl::Variable<T> > est_catch_fsh1;
    atl::Vector<atl::Variable<T> > est_catch_fsh2;
    atl::Matrix<atl::Variable<T> > est_fsh1_prop_at_age;
    atl::Matrix<atl::Variable<T> > est_fsh2_prop_at_age;
    atl::Vector<atl::Variable<T> > est_srv1_biomass;
    atl::Vector<atl::Variable<T> > est_srv2_biomass;
    atl::Matrix<atl::Variable<T> > est_srv1_prop_at_age;
    atl::Matrix<atl::Variable<T> > est_srv2_prop_at_age;

    atl::Matrix<atl::Variable<T> > est_total_biomass;   // area x year
    atl::Matrix<atl::Variable<T> > est_spawn_biomass;   // area x year

    atl::Vector<atl::Variable<T> > nll_parts;

    int max_loop_count = 0;     // maximum number of loops for the inner optimization

public:

    CatchAtAge() { }

    void RegisterHyperParameterVector(atl::Vector<atl::Variable<T> >& p_vec, const int phase = 1)
    {
        for (int i = 0; i < p_vec.Size(0); i++)
        {
            this->RegisterHyperParameter(p_vec(i),phase);
        }

    }

    void Initialize(int loop_count)
    {
        // for inner optimization
        if (loop_count > 0 && loop_count < 1000)
        {
            max_loop_count = loop_count;
        }

        maturity.Resize(nages);

        wt_spawn.Resize(nages);
        wt_fsh.Resize(nages);
        wt_srv.Resize(nages);
        wt_pop.Resize(nages);

        fsh_sel.Resize(nareas,nages);
        srv_sel.Resize(nareas,nages);

        est_catch_fsh1.Resize(nyrs);
        est_catch_fsh2.Resize(nyrs);
        est_fsh1_prop_at_age.Resize(nyrs_fsh_prop_at_age, nages);
        est_fsh2_prop_at_age.Resize(nyrs_fsh_prop_at_age, nages);

        est_srv1_biomass.Resize(nyrs_srv);
        est_srv2_biomass.Resize(nyrs_srv);
        est_srv1_prop_at_age.Resize(nyrs_srv_prop_at_age, nages);
        est_srv2_prop_at_age.Resize(nyrs_srv_prop_at_age, nages);

        est_total_biomass.Resize(nareas, nyrs);
        est_spawn_biomass.Resize(nareas, nyrs);


        nll_parts.Resize(25);


        this->RegisterHyperParameterVector(log_fsh_sel_alpha,4);
        this->RegisterHyperParameterVector(log_fsh_sel_beta,4);
        log_fsh_sel_alpha.SetBounds(0.0,2.0);
        log_fsh_sel_beta.SetBounds(-5.0,5.0);
        log_fsh_sel_alpha  = atl::Variable<T>(1.3863);
        log_fsh_sel_beta   = atl::Variable<T>(1.0);


        this->RegisterHyperParameterVector(log_srv_sel_alpha,7);
        this->RegisterHyperParameterVector(log_srv_sel_beta,7);
        log_srv_sel_alpha.SetBounds(-1.0,5.0);
        log_srv_sel_beta.SetBounds(-5.0,5.0);
        log_srv_sel_alpha = atl::Variable<T>(0.0);
        log_srv_sel_beta  = atl::Variable<T>(4.0);


        this->RegisterHyperParameterVector(log_srv_q,5);
        log_srv_q.SetBounds(-10.0,10.0);
        log_srv_q = atl::Variable<T>(0.0);


        this->RegisterHyperParameterVector(log_R0,1);
        log_R0.SetBounds(5.0,36.0);
        log_R0 = atl::Variable<T>(20.7232658369);

        this->RegisterHyperParameterVector(log_h,1);
        log_h.SetBounds(0.2,1.0);
        log_h = atl::Variable<T>(0.75);


        this->RegisterHyperParameterVector(log_mean_fsh_mort,1);
        log_mean_fsh_mort.SetBounds(-10.0,10.0);
        log_mean_fsh_mort = atl::Variable<T>(-2.30258509299);


        init_pop_devs.Resize(nareas, nages-1);
        initial_population.Resize(nareas, nages);

        recruit_devs.Resize(nareas, nyrs);
        recruits.Resize(nareas, nyrs+1);

        fsh_mort_devs.Resize(nareas, nyrs);
        fsh_mort.Resize(nareas, nyrs);


        // this->RegisterHyperParameter(init_pop_devs,4);
        // this->RegisterHyperParameterVector(init_pop_devs,4);
        // init_pop_devs.SetBounds(-15.0,15.0);
        init_pop_devs = atl::Variable<T>(0.0);

        // this->RegisterHyperParameter(recruit_devs,3);
        // this->RegisterHyperParameterVector(recruit_devs,3);
        // recruit_devs.SetBounds(-15.0,15.0);
        recruit_devs = atl::Variable<T>(0.0);

        // this->RegisterHyperParameter(fsh_mort_devs,2);
        this->RegisterHyperParameterVector(fsh_mort_devs,2);
        fsh_mort_devs.SetBounds(-10.0,10.0);
        fsh_mort_devs = atl::Variable<T>(0.0);
    }

    void Maturity()
    {
        mat_alpha = atl::exp(log_mat_alpha);
        mat_beta  = atl::exp(log_mat_beta);

        for ( int a = 0; a < nages; ++a )
        {
            maturity(a) = 1.0 / (1.0 + atl::exp(-1.0 * ((T(ages(a)) - mat_alpha) / mat_beta)));
        }
    }

    void Selectivity()
    {
        fsh_sel = 0.0;

        fsh_sel_alpha  = atl::exp(log_fsh_sel_alpha);
        fsh_sel_beta   = atl::exp(log_fsh_sel_beta);

        for ( int d = 0; d < nareas; d++ )
        {
            for ( int a = 0; a < nages; a++ )
            {
                fsh_sel(d, a) = 1.0 / (1.0 + atl::exp(-1.0 * ((T(ages(a)) - fsh_sel_alpha) / fsh_sel_beta)));
            }
        }

        srv_sel = 0.0;

        srv_sel_alpha  = atl::exp(log_srv_sel_alpha);
        srv_sel_beta   = atl::exp(log_srv_sel_beta);

        for ( int d = 0; d < nareas; d++ )
        {
            for ( int a = 0; a < nages ; a++ )
            {
                srv_sel(d, a) = 1.0 / (1.0 + atl::exp(-1.0 * ((T(ages(a)) - srv_sel_alpha) / srv_sel_beta)));
            }
        }
    }

    void Mortality()
    {
        for ( int d = 0; d < nareas; ++d )
        {
            for ( int y = 0; y < nyrs; ++y )
            {
                fsh_mort(d, y) = atl::exp(log_mean_fsh_mort(d) + fsh_mort_devs(d, y));

                for (int a = 0; a < nages; ++a)
                {
                    F(d, y, a) = fsh_mort(d, y) * fsh_sel(d, a);
                }
            }
        }

        for ( int d = 0; d < nareas; ++d )
        {
            for ( int y = 0; y < nyrs; ++y )
            {
                for ( int a = 0; a < nages; ++a )
                {
                    Z(d, y, a) = F(d, y, a) + M(a);
                }
            }
        }
    }

    void Survivability()
    {
        for ( int d = 0; d < nareas; ++d )
        {
            for ( int y = 0; y < nyrs; ++y )
            {
                for ( int a = 0; a < nages; ++a )
                {
                    expZ(d, y, a) = atl::exp(-1.0 * Z(d, y, a));

                    expZ_sp(d, y, a) = atl::exp(-1.0 * yrfrac_sp * Z(d, y, a));

                    expZ_yrfrac_srv(d, y, a) = atl::exp(-1.0 * yrfrac_srv * Z(d, y, a));
                }
            }
        }
    }

    atl::Vector<atl::Variable<T> > length_at_age(atl::Variable<T> yrfrac)
    {
        atl::Vector<atl::Variable<T> > laa_vec;
        laa_vec.Resize(nages);

        for ( int a = 0; a < nages; ++a )
        {
            laa_vec(a) = L_Amin + ((L_Amax - L_Amin) * (1.0 - atl::pow(c, (T(ages(a)) + yrfrac - Amin))) / (1.0 - atl::pow(c, (Amax - Amin))));

            if (laa_vec(a) < 0.00001)
            {
                laa_vec(a) = 0.00001;
            }
        }

        return laa_vec;
    }

    atl::Vector<atl::Variable<T> > weight_at_age(atl::Variable<T> yrfrac)
    {
        atl::Vector<atl::Variable<T> > laa_vec = length_at_age(area, yrfrac);

        atl::Vector<atl::Variable<T> > waa_vec;
        waa_vec.Resize(nages);

        for ( int a = 0; a < nages; ++a )
        {
            waa_vec(a) = wtAA_A * atl::pow(laa_vec(a), wtAA_B);
        }

        return waa_vec;
    }

    void NumbersAtAge()
    {
        wt_pop = weight_at_age(atl::Variable<T>(0.0));
        wt_spawn = weight_at_age(yrfrac_sp);

        // initial numbers-at-age by area
        for ( int d = 0; d < nareas ; ++d )
        {
            initial_population(d, 0) = atl::exp(log_R0(d));
            for ( int a = 1; a < nages; ++a )
            {
                initial_population(d, a) = initial_population(d, a-1) * atl::exp(atl::Variable<T>(-1.0 * M(a-1)));
            }
            initial_population(d, nages-1) /= (1.0 - atl::exp(atl::Variable<T>(-1.0 * M(nages-1))));
        }

        // inner redistribution loop
        if (max_loop_count > 0)
        {

        }

        // apply devs
        for ( int d = 0; d < nareas ; ++d )
        {
            for ( int a = 1; a < nages; ++a )
            {
                initial_population(d, a) *= atl::exp(init_pop_devs(d, a-1));
            }
        }

        // calculate unfished equilibrium female spawning biomass
        SB0 = 0.0;
        for ( int d = 0; d < nareas; ++d )
        {
            for ( int a = 0; a < nages; ++a )
            {
                SB0(d) += (maturity(a) * wt_spawn(a) * initial_population(d, a) * atl::exp(-1.0 * yrfrac_sp * M(a)));
            }

            SB0(d) *= atl::Variable<T>(0.5);    // females only
        }

        // std::cout << "init pop " << initial_population << std::endl;

        N(0) = initial_population;

        // could put a S-R relationship here
        // N(0, 0) = something;

        // std::cout << "first year N-at-age " << atl::Row(N, 0) << std::endl;

        for  (int d = 0; d < nareas; ++d )
        {
            for ( int y = 0; y < nyrs; ++y )
            {
                N(d, y, 0) *= atl::exp(recruit_devs(d, y));

                // this assumes continuous fishing
                for ( int a = 1; a < nages; ++a )
                {
                    N(d, y+1, a) = N(d, y, a-1) * expZ(d, y, a-1);
                }
                N(d, y+1, (nages-1)) += (N(d, y, (nages-1)) * expZ(d, y, (nages-1)));

                N(d, y+1, 0) = atl::exp(log_R0(d));

                // std::cout << "N-at-age in year " << i << " " << atl::Row(N, i) << std::endl;

                // calculate total and spawning biomass - vector operations
                est_total_biomass(d, y)   = T(0);
                est_spawn_biomass(d, y)   = T(0);
                n_at_age_row = atl::Row(N, i);

                for ( int a = 0; a < nages; ++a )
                {
                    est_total_biomass(d, y) += (wt_pop(a) * N(d, y, a));

                    est_spawn_biomass(d, y) += (0.5 * maturity(a) * wt_spawn(a) * N(d, y, a) * expZ_sp(d, y, a));
                }
                est_total_biomass(d, y)   /= 1000.0;
                est_summary_biomass(d, y) /= 1000.0;
                est_spawn_biomass(d, y)   /= 1000.0;
            }

            recruits(d) = atl::Column(N,0);
        }

        // convert from kg to mt
        // est_total_biomass /= 1000.0;
        // est_spawn_biomass /= 1000.0;
    }

    void FleetIndices()
    {
        atl::Variable<T> est_srv_num;
        int y;

        srv_q  = atl::exp(log_srv_q);

        wt_srv = weight_at_age(yrfrac_srv);

        for ( int d = 0; d < nareas; ++d )
        {
            for ( int i = 0; i < nyrs_srv; ++i )
            {
                y = yrs_srv(i);

                est_srv1_biomass(y) = srv_q(0) * atl::Sum(atl::Row(obs_srv1_wt_at_age, y) * srv_sel_row * atl::Row(N, y) * atl::Row(expZ_yrfrac_srv1, y)) / 1000.0;
            }

            // calculate proportions at age
            for ( int i = 0; i < nyrs_srv1_prop_at_age; i++ )
            {
                y = yrs_srv1_prop_at_age(i);

                // est_srv_1_prop_at_age(i) = atl::Vector<atl::Variable<T> >(age_age_err * (srv_sel(0) * N(y) * expZ_yrfrac_srv1(y)));
                n_at_age_row = atl::Row(N, y) * atl::Row(expZ_yrfrac_srv1, y);

                for ( int j = 0; j < nages; j++ )
                {
                    est_srv_1_prop_at_age(i,j) = atl::Sum(srv_sel_row * n_at_age_row * atl::Column(age_age_err,j));
                }

                est_srv_num = atl::Sum(atl::Row(est_srv_1_prop_at_age, i));
                if ( est_srv_num > T(0) )
                {
                    est_srv_1_prop_at_age(i) /= est_srv_num;
                }
            }
        }
    }

    void CalculateCatchAtAge()
    {
        atl::Vector<atl::Variable<T> > prop_at_age_row;
        atl::Variable<T> est_catch_num;

        wt_fsh = weight_at_age(yrfrac_fsh);

        for ( int i = 0; i < nyrs; i++ )
        {
            est_catch(i)           = T(0);
            est_fsh_prop_at_age(i) = T(0);
            est_fsh_prop_at_len(i) = T(0);

            for ( int j = 0; j < nages; j++ )
            {
                C(i,j) = atl::Sum(((atl::Row(F,i) / atl::Row(Z,i)) * (1.0 - atl::Row(expZ,i)) * atl::Row(N,i)) * atl::Column(age_age_err,j));
                est_catch(i) += (obs_fsh_wt_at_age(i,j) * C(i,j));
            }
            est_catch(i) /= 1000.0;

            // calculate proportions at age
            est_catch_num = atl::Sum(atl::Row(C,i));
            if ( est_catch_num > T(0) )
            {
                est_fsh_prop_at_age(i) = C(i) / est_catch_num;
            }

            // calculate proportions at length
            // est_fsh_prop_at_len(i) = atl::Vector<atl::Variable<T> >(est_fsh_prop_at_age(i) * age_len_trans_1);
            prop_at_age_row = atl::Row(est_fsh_prop_at_age,i);
            for ( int j = 0; j < n_fsh_len_bins; j++ )
            {
                est_fsh_prop_at_len(i,j) = atl::Sum(prop_at_age_row * atl::Column(age_len_trans_1,j));
            }
        }
    }

   void PrepareDeviations()
   {
        // if (this->Phase() >= this->GetActivePhase(init_pop_devs(0)))
        {
            init_pop_devs -= (atl::Sum(init_pop_devs) / (double) init_pop_devs.Size(0));
        }

        // if (this->Phase() >= this->GetActivePhase(recruit_devs(0)))
        {
            recruit_devs -= (atl::Sum(recruit_devs) / (double) recruit_devs.Size(0));
        }

        // if (this->Phase() >= this->GetActivePhase(fsh_mort_devs(0)))
        {
            fsh_mort_devs -= (atl::Sum(fsh_mort_devs) / (double) fsh_mort_devs.Size(0));
        }
    }

    virtual const atl::Variable<T> Evaluate()
    {
       atl::Variable<T> o = 0.00001;  // small value for proportions calculations

        atl::Variable<T> f = 0;

        this->PrepareDeviations();

        this->Maturity();
        this->Selectivity();
        this->Mortality();
        this->Survivability();
        this->NumbersAtAge();
        this->FleetIndices();
        this->CalculateCatchAtAge();

        // NLL for catch
        nll_parts(0) = 0;
        for ( int i = 0; i < nyrs; i++ )
        {
            nll_parts(0) += (0.5 * SQUARE((atl::log(obs_catch(i) + o) - atl::log(est_catch(i) + o)) / obs_catch_CV(i)));
        }

        // NLL for fsh proportions at age
        nll_parts(1) = 0;
        for ( int i = 0; i < nyrs_fsh_prop_at_age; i++ )
        {
            if ( obs_fsh_prop_at_age_N(i) > 0 )
            {
                int y = yrs_fsh_prop_at_age(i);

                for ( int j = age_young_fsh; j <= age_old_fsh; j++ )
                {
                    nll_parts(1) -= (T(obs_fsh_prop_at_age_N(i)) * obs_fsh_prop_at_age(i, j) * (atl::log(est_fsh_prop_at_age(y, j) + o) - atl::log(obs_fsh_prop_at_age(i, j) + o)));
                }
            }
        }

        // NLL for srv 1 biomass
        nll_parts(3) = 0;

        for ( int i = 0; i < nyrs_srv1; i++ )
        {
            nll_parts(3) += (0.5 * SQUARE(((atl::log(obs_srv_1_biomass(i) + o) - atl::log(est_srv_1_biomass(i) + o) + SQUARE((obs_srv_1_CV(i) / 2.0))) / obs_srv_1_CV(i))));
        }

        // NLL for srv 1 proportions at age
        nll_parts(4) = 0;
        for ( int i = 0; i < nyrs_srv1_prop_at_age; i++ )
        {
            if ( obs_srv_1_prop_at_age_N(i) > 0 )
            {
                for ( int j = age_young_srv1; j <= age_old_srv1; j++ )
                {
                    nll_parts(4) -= (T(obs_srv_1_prop_at_age_N(i)) * obs_srv_1_prop_at_age(i, j) * (atl::log(est_srv_1_prop_at_age(i, j) + o) - atl::log(obs_srv_1_prop_at_age(i, j) + o)));
                }
            }
        }

        // penalty on initial population devs
        // nll_parts(17) = (0.5 / SQUARE(sigmaR)) * atl::Norm2(init_pop_devs);

        // penalty on rec devs
        // nll_parts(18) = (0.5 / SQUARE(sigmaR)) * atl::Norm2(recruit_devs);
        nll_parts(18) = 0.0;
        for ( int i = 0; i <= 7; i++ )
        {
            nll_parts(18) += (0.5 * SQUARE(recruit_devs(i) / sigmaR));
        }
        for ( int i = (nyrs - 2); i < nyrs; i++ )
        {
            nll_parts(18) += (0.5 * SQUARE(recruit_devs(i) / sigmaR));
        }

        // penalty on fsh mort dev
        // nll_parts(19) = atl::Norm2(fsh_mort_devs);

        f = atl::Sum(nll_parts);

        return f;
    }

    void Report()
    {
        atl::Variable<T> one_meeellion = 1000000.0;     // conversion factor

        std::cout << std::endl << std::endl;
        std::cout << "objective function " << atl::Sum(nll_parts) << std::endl;
        std::cout << "objective function components " << nll_parts << std::endl;
        std::cout << std::endl;

        std::cout << "init devs " << init_pop_devs << std::endl;
        std::cout << "init N-at-age " << initial_population << std::endl;
        std::cout << "ratios " << (initial_population / initial_population(0)) << std::endl;
        std::cout << std::endl;

        std::cout << "log R0 " << log_R0 << "\t" << atl::exp(log_R0) << std::endl;
        std::cout << "recruit_devs " << recruit_devs << std::endl;
        std::cout << "recruits " << recruits << std::endl;
        std::cout << std::endl;

        std::cout << "log mean_fsh_mort " << log_mean_fsh_mort << "\t" << atl::exp(log_mean_fsh_mort) << std::endl;
        std::cout << "fsh_mort_devs " << fsh_mort_devs << std::endl;
        std::cout << "fsh_mort " << fsh_mort << std::endl;
        std::cout << std::endl;

        std::cout << "log fsh_sel_alpha " << log_fsh_sel_alpha << std::endl;
        std::cout << "log fsh_sel_beta " << log_fsh_sel_beta << std::endl;
        std::cout << "fsh_sel_alpha " << fsh_sel_alpha << std::endl;
        std::cout << "fsh_sel_beta " << fsh_sel_beta << std::endl;
        std::cout << std::endl;
        std::cout << "fsh sel" << std::endl;
        std::cout << fsh_sel << std::endl;
        std::cout << std::endl;

        std::cout << "observed catch " << obs_catch << std::endl;
        std::cout << "estimated catch " << est_catch << std::endl;
        std::cout << std::endl;

        std::cout << "srv_1_q " << srv_1_q << std::endl;
        std::cout << std::endl;

        std::cout << "log_srv1_sel_alpha " << log_srv1_sel_asc_alpha << std::endl;
        std::cout << "log_srv1_sel_beta " << log_srv1_sel_asc_beta << std::endl;
        std::cout << "srv1_sel_alpha " << srv1_sel_asc_alpha << std::endl;
        std::cout << "srv1_sel_beta " << srv1_sel_asc_beta << std::endl;
        std::cout << std::endl;

        for ( int k = 0; k < nsrvs; k++ )
        {
            std::cout << "srv_sel " << (k + 1) << " " << atl::Row(srv_sel, k) << std::endl;
        }
        std::cout << std::endl;

        std::cout << "observed srv 1 " << obs_srv_1_biomass << std::endl;
        std::cout << "estimated srv 1 " << est_srv_1_biomass << std::endl;
        std::cout << std::endl;

        std::cout << "total biomass " << est_total_biomass << std::endl;
        std::cout << "spawning biomass " << est_spawn_biomass << std::endl;
        std::cout << std::endl;

        std::cout << "N at age" << std::endl;
        std::cout << (N / one_meeellion) << std::endl;
        std::cout << std::endl;

        std::cout << "Observed fishery proportions-at-age" << std::endl;
        std::cout << obs_fsh_prop_at_age << std::endl;
        std::cout << "Estimated fishery proportions-at-age" << std::endl;
        std::cout << est_fsh_prop_at_age << std::endl;
        std::cout << std::endl;

        std::cout << "Observed survey 1 proportions-at-age" << std::endl;
        std::cout << obs_srv_1_prop_at_age << std::endl;
        std::cout << "Estimated survey 1 proportions-at-age" << std::endl;
        std::cout << est_srv_1_prop_at_age << std::endl;
        std::cout << std::endl;

        std::cout << std::endl;
    }

    void CalculateHarvestStrategy()
    {
    }

    void GetVarCovar()
    {
        // atl::Matrix<T> hess = this->Hessian();
        atl::Matrix<T> hess = atl::Matrix<T>::Identity(10);    // for testing
        atl::Matrix<T> inverse_hess = atl::Matrix<T>::Identity(hess.Size(0));

        std::cout << "\nHessian matrix\n" << hess << std::endl;

        hess.Invert();
        inverse_hess = hess;

        std::cout << "\nInverse of Hessian matrix\n" << inverse_hess << std::endl;

        atl::Vector<T> se(hess.Size(0));

        for (int i = 0; i < hess.Size(0); i++)
        {
            se(i) = std::sqrt(inverse_hess(i,i));
        }

        std::cout << "\nSE vector\n" << se << std::endl;

        atl::Matrix<T> var_covar = inverse_hess / (atl::OuterProduct(se,se));

        std::cout << "\nVariance-covariance matrix\n" << var_covar << std::endl;
    }

};


#endif /* CATCHATAGE_HPP */
