function test_emg2    
    clear;
    [sig, fs, tm] = rdsamp('.\Signals\emg_myopathy', 1);
    stm=serial('COM2', 'BaudRate', 115200);
    fopen(stm);
    faktor=round(fs/750);
    fs = fs/faktor;
    sig = downsample(sig, faktor);
    fn=fs/2;
    sig=sig';
    sig=sig(1:6000);
    sig=sig-mean(sig);

    lowp=fir1(20, 380/fn,'low');
    highp=fir1(20, 10/fn,'high');
    % fprintf('%s\n',sprintf(' %.8ff,',lowp));
    % fprintf('%s\n',sprintf(' %.8ff,',highp));
    
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
    fwrite(stm, 'M');
    fclose(stm);
end