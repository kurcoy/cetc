class Converter(object):
    @staticmethod
    def hexToascii(h):
        list_s = []
        for i in range(0, len(h), 2):
            list_s.append(chr(int(h[i:i+2], 16)))
        return ''.join(list_s)

    @staticmethod
    def asciiTohex(s):
        list_h = []
        for c in s:
            list_h.append(str(hex(ord(c))[2:]))
        return ''.join(list_h)

    def byteTohex( bins ):
        return ''.join( [ "%02X" % x for x in bins ] ).strip()
  
