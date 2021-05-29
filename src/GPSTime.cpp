    // // get time from GPS module
    //   if (gps.time.isValid())
    //   {
    //     Minute = gps.time.minute();
    //     Second = gps.time.second();
    //     Hour   = gps.time.hour();
    //   }
 
    //   // get date drom GPS module
    //   if (gps.date.isValid())
    //   {
    //     Day   = gps.date.day();
    //     Month = gps.date.month();
    //     Year  = gps.date.year();
    //   }

    // if(last_second != gps.time.second())  // if time has changed
    // {
    //     last_second = gps.time.second();

    //     // set current UTC time
    //     setTime(Hour, Minute, Second, Day, Month, Year);
    //     // add the offset to get local time
    //     adjustTime(time_offset);

    //     // update time array
    //     Time[12] = second() / 10 + '0';
    //     Time[13] = second() % 10 + '0';
    //     Time[9]  = minute() / 10 + '0';
    //     Time[10] = minute() % 10 + '0';
    //     Time[6]  = hour()   / 10 + '0';
    //     Time[7]  = hour()   % 10 + '0';

    //     // update date array
    //     Date[14] = (year()  / 10) % 10 + '0';
    //     Date[15] =  year()  % 10 + '0';
    //     Date[9]  =  month() / 10 + '0';
    //     Date[10] =  month() % 10 + '0';
    //     Date[6]  =  day()   / 10 + '0';
    //     Date[7]  =  day()   % 10 + '0';

    //     // print time & date
    //     print_wday(weekday());   // print day of the week
    //     lcd.setCursor(0, 2);     // move cursor to column 0 row 2
    //     lcd.print(Time);         // print time (HH:MM:SS)
    //     lcd.setCursor(0, 3);     // move cursor to column 0 row 3
    //     lcd.print(Date);         // print date (DD-MM-YYYY)
    // }