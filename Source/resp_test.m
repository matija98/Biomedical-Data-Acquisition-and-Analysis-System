function resp_test
    clear;
    load('.\Signals\17_resp.mat')
    stm=serial('COM2', 'BaudRate', 115200);
    fopen(stm);
    faktor=round(Fs/500);
    fs = Fs/faktor;
    sig = downsample(D, faktor);
    fn=fs/2;
    sig=sig(1:12000);
    sig=sig-mean(sig);

    lowp=fir1(20, 4/fn,'low');
    highp=fir1(20, 0.1/fn,'high');
    %fprintf('%s\n',sprintf(' %.8ff,',lowp));
    %fprintf('%s\n',sprintf(' %.8ff,',highp));
    
    a=fs;
    a=num2str(a,'%.1f');
    a=strcat(a, 'X');
    fwrite(stm, a);
    for dio = sig
        a=round(dio,4);
        a=num2str(a,'%.4f');
        a=strcat(a, 'X');
        fwrite(stm, a);
    end
    fwrite(stm, 'R');
    fclose(stm);
end