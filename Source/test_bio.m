function test_bio
    clear;
    [sig, fs, tm] = rdsamp('.\Signals\118e06', 1);
    stm=serial('COM2', 'BaudRate', 115200);
    fopen(stm);
    fn=fs/2;
    sig=sig';
    sig=sig(1:12000);
    sig=sig-mean(sig);

    lowp=fir1(20, 5/fn,'low');
    highp=fir1(20, 0.2/fn,'high');
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
    fwrite(stm, 'C');
    fclose(stm);
end