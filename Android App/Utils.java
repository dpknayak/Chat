package com.example.chat;

public class Utils {
    public static String msgToTLV(String messge, String id) {
        String result = "15" + id + "2";

        int len = messge.length();
        if (len < 10)
            result += "00";
        else if (len < 100)
            result += "0";

        result += String.valueOf(len);
        result += messge;
        return result;
    }

    public static boolean validIP(String ip) {
        try {
            if (ip == null || ip.isEmpty()) {
                return false;
            }
            String[] parts = ip.split("\\.");
            if (parts.length != 4) {
                return false;
            }
            for (String s : parts) {
                int i = Integer.parseInt(s);
                if ((i < 0) || (i > 255)) {
                    return false;
                }
            }
            if (ip.endsWith(".")) {
                return false;
            }
            return true;
        } catch (NumberFormatException nfe) {
            return false;
        }
    }

    public static boolean validId(String id) {
        int i = Integer.parseInt(id);
        if ((i < 10000) || (i > 99999)) {
            return false;
        }
        return true;
    }

}
