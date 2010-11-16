#import "Entities.h"

/* Taken from http://www.w3.org/TR/REC-html40/sgml/entities.html */
static struct {
	String entity;
	int c;
} entities[] = {
	{ String("AElig"), 198 },
	{ String("Aacute"), 193 },
	{ String("Acirc"), 194 },
	{ String("Agrave"), 192 },
	{ String("Alpha"), 913 },
	{ String("Aring"), 197 },
	{ String("Atilde"), 195 },
	{ String("Auml"), 196 },
	{ String("Beta"), 914 },
	{ String("Ccedil"), 199 },
	{ String("Chi"), 935 },
	{ String("Dagger"), 8225 },
	{ String("Delta"), 916 },
	{ String("ETH"), 208 },
	{ String("Eacute"), 201 },
	{ String("Ecirc"), 202 },
	{ String("Egrave"), 200 },
	{ String("Epsilon"), 917 },
	{ String("Eta"), 919 },
	{ String("Euml"), 203 },
	{ String("Gamma"), 915 },
	{ String("Iacute"), 205 },
	{ String("Icirc"), 206 },
	{ String("Igrave"), 204 },
	{ String("Iota"), 921 },
	{ String("Iuml"), 207 },
	{ String("Kappa"), 922 },
	{ String("Lambda"), 923 },
	{ String("Mu"), 924 },
	{ String("Ntilde"), 209 },
	{ String("Nu"), 925 },
	{ String("OElig"), 338 },
	{ String("Oacute"), 211 },
	{ String("Ocirc"), 212 },
	{ String("Ograve"), 210 },
	{ String("Omega"), 937 },
	{ String("Omicron"), 927 },
	{ String("Oslash"), 216 },
	{ String("Otilde"), 213 },
	{ String("Ouml"), 214 },
	{ String("Phi"), 934 },
	{ String("Pi"), 928 },
	{ String("Prime"), 8243 },
	{ String("Psi"), 936 },
	{ String("Rho"), 929 },
	{ String("Scaron"), 352 },
	{ String("Sigma"), 931 },
	{ String("THORN"), 222 },
	{ String("Tau"), 932 },
	{ String("Theta"), 920 },
	{ String("Uacute"), 218 },
	{ String("Ucirc"), 219 },
	{ String("Ugrave"), 217 },
	{ String("Upsilon"), 933 },
	{ String("Uuml"), 220 },
	{ String("Xi"), 926 },
	{ String("Yacute"), 221 },
	{ String("Yuml"), 376 },
	{ String("Zeta"), 918 },
	{ String("aacute"), 225 },
	{ String("acirc"), 226 },
	{ String("acute"), 180 },
	{ String("aelig"), 230 },
	{ String("agrave"), 224 },
	{ String("alefsym"), 8501 },
	{ String("alpha"), 945 },
	{ String("amp"), 38 },
	{ String("and"), 8743 },
	{ String("ang"), 8736 },
	{ String("aring"), 229 },
	{ String("asymp"), 8776 },
	{ String("atilde"), 227 },
	{ String("auml"), 228 },
	{ String("bdquo"), 8222 },
	{ String("beta"), 946 },
	{ String("brvbar"), 166 },
	{ String("bull"), 8226 },
	{ String("cap"), 8745 },
	{ String("ccedil"), 231 },
	{ String("cedil"), 184 },
	{ String("cent"), 162 },
	{ String("chi"), 967 },
	{ String("circ"), 710 },
	{ String("clubs"), 9827 },
	{ String("cong"), 8773 },
	{ String("copy"), 169 },
	{ String("crarr"), 8629 },
	{ String("cup"), 8746 },
	{ String("curren"), 164 },
	{ String("dArr"), 8659 },
	{ String("dagger"), 8224 },
	{ String("darr"), 8595 },
	{ String("deg"), 176 },
	{ String("delta"), 948 },
	{ String("diams"), 9830 },
	{ String("divide"), 247 },
	{ String("eacute"), 233 },
	{ String("ecirc"), 234 },
	{ String("egrave"), 232 },
	{ String("empty"), 8709 },
	{ String("emsp"), 8195 },
	{ String("ensp"), 8194 },
	{ String("epsilon"), 949 },
	{ String("equiv"), 8801 },
	{ String("eta"), 951 },
	{ String("eth"), 240 },
	{ String("euml"), 235 },
	{ String("euro"), 8364 },
	{ String("exist"), 8707 },
	{ String("fnof"), 402 },
	{ String("forall"), 8704 },
	{ String("frac12"), 189 },
	{ String("frac14"), 188 },
	{ String("frac34"), 190 },
	{ String("frasl"), 8260 },
	{ String("gamma"), 947 },
	{ String("ge"), 8805 },
	{ String("gt"), 62 },
	{ String("hArr"), 8660 },
	{ String("harr"), 8596 },
	{ String("hearts"), 9829 },
	{ String("hellip"), 8230 },
	{ String("iacute"), 237 },
	{ String("icirc"), 238 },
	{ String("iexcl"), 161 },
	{ String("igrave"), 236 },
	{ String("image"), 8465 },
	{ String("infin"), 8734 },
	{ String("int"), 8747 },
	{ String("iota"), 953 },
	{ String("iquest"), 191 },
	{ String("isin"), 8712 },
	{ String("iuml"), 239 },
	{ String("kappa"), 954 },
	{ String("lArr"), 8656 },
	{ String("lambda"), 955 },
	{ String("lang"), 9001 },
	{ String("laquo"), 171 },
	{ String("larr"), 8592 },
	{ String("lceil"), 8968 },
	{ String("ldquo"), 8220 },
	{ String("le"), 8804 },
	{ String("lfloor"), 8970 },
	{ String("lowast"), 8727 },
	{ String("loz"), 9674 },
	{ String("lrm"), 8206 },
	{ String("lsaquo"), 8249 },
	{ String("lsquo"), 8216 },
	{ String("lt"), 60 },
	{ String("macr"), 175 },
	{ String("mdash"), 8212 },
	{ String("micro"), 181 },
	{ String("middot"), 183 },
	{ String("minus"), 8722 },
	{ String("mu"), 956 },
	{ String("nabla"), 8711 },
	{ String("nbsp"), 160 },
	{ String("ndash"), 8211 },
	{ String("ne"), 8800 },
	{ String("ni"), 8715 },
	{ String("not"), 172 },
	{ String("notin"), 8713 },
	{ String("nsub"), 8836 },
	{ String("ntilde"), 241 },
	{ String("nu"), 957 },
	{ String("oacute"), 243 },
	{ String("ocirc"), 244 },
	{ String("oelig"), 339 },
	{ String("ograve"), 242 },
	{ String("oline"), 8254 },
	{ String("omega"), 969 },
	{ String("omicron"), 959 },
	{ String("oplus"), 8853 },
	{ String("or"), 8744 },
	{ String("ordf"), 170 },
	{ String("ordm"), 186 },
	{ String("oslash"), 248 },
	{ String("otilde"), 245 },
	{ String("otimes"), 8855 },
	{ String("ouml"), 246 },
	{ String("para"), 182 },
	{ String("part"), 8706 },
	{ String("permil"), 8240 },
	{ String("perp"), 8869 },
	{ String("phi"), 966 },
	{ String("pi"), 960 },
	{ String("piv"), 982 },
	{ String("plusmn"), 177 },
	{ String("pound"), 163 },
	{ String("prime"), 8242 },
	{ String("prod"), 8719 },
	{ String("prop"), 8733 },
	{ String("psi"), 968 },
	{ String("quot"), 34 },
	{ String("rArr"), 8658 },
	{ String("radic"), 8730 },
	{ String("rang"), 9002 },
	{ String("raquo"), 187 },
	{ String("rarr"), 8594 },
	{ String("rceil"), 8969 },
	{ String("rdquo"), 8221 },
	{ String("real"), 8476 },
	{ String("reg"), 174 },
	{ String("rfloor"), 8971 },
	{ String("rho"), 961 },
	{ String("rlm"), 8207 },
	{ String("rsaquo"), 8250 },
	{ String("rsquo"), 8217 },
	{ String("sbquo"), 8218 },
	{ String("scaron"), 353 },
	{ String("sdot"), 8901 },
	{ String("sect"), 167 },
	{ String("shy"), 173 },
	{ String("sigma"), 963 },
	{ String("sigmaf"), 962 },
	{ String("sim"), 8764 },
	{ String("spades"), 9824 },
	{ String("sub"), 8834 },
	{ String("sube"), 8838 },
	{ String("sum"), 8721 },
	{ String("sup"), 8835 },
	{ String("sup1"), 185 },
	{ String("sup2"), 178 },
	{ String("sup3"), 179 },
	{ String("supe"), 8839 },
	{ String("szlig"), 223 },
	{ String("tau"), 964 },
	{ String("there4"), 8756 },
	{ String("theta"), 952 },
	{ String("thetasym"), 977 },
	{ String("thinsp"), 8201 },
	{ String("thorn"), 254 },
	{ String("tilde"), 732 },
	{ String("times"), 215 },
	{ String("trade"), 8482 },
	{ String("uArr"), 8657 },
	{ String("uacute"), 250 },
	{ String("uarr"), 8593 },
	{ String("ucirc"), 251 },
	{ String("ugrave"), 249 },
	{ String("uml"), 168 },
	{ String("upsih"), 978 },
	{ String("upsilon"), 965 },
	{ String("uuml"), 252 },
	{ String("weierp"), 8472 },
	{ String("xi"), 958 },
	{ String("yacute"), 253 },
	{ String("yen"), 165 },
	{ String("yuml"), 255 },
	{ String("zeta"), 950 },
	{ String("zwj"), 8205 },
	{ String("zwnj"), 8204 }
};

sdef(String, Decode, String s) {
	String res = HeapString(s.len);

	bool ampersand = false;
	size_t pos = 0;

	for (size_t i = 0; i < s.len; i++) {
		if (!ampersand) {
			if (s.buf[i] == '&') {
				pos = i + 1;
				ampersand = true;
			} else {
				String_Append(&res, s.buf[i]);
			}
		} else {
			if (s.buf[i] == ';') {
				String entity = String_Slice(s, pos, i - pos);

				if (entity.buf[0] == '#') {
					if (entity.len < 3) {
						goto error;
					}

					s32 c;
					if (entity.buf[1] == 'x') {
						c = Hex_ToInteger(String_Slice(entity, 2));
					} else {
						c = Int32_Parse(String_Slice(entity, 1));
					}

					String s = StackString(4);
					Unicode_ToMultiByte(c, &s);

					String_Append(&res, s);
				} else {
					size_t j;

					for (j = 0; j < nElems(entities); j++) {
						if (String_Equals(entities[j].entity, entity)) {
							goto found;
						}
					}

					goto error;

					when (found) {
						String s = StackString(4);
						Unicode_ToMultiByte(entities[j].c, &s);

						String_Append(&res, s);
					}
				}

				ampersand = false;
			} else if (Char_IsSpace(s.buf[i]) || i == s.len - 1) {
				goto error;
			}

			when (error) {
				String_Append(&res, String_Slice(s, pos - 1, i - pos + 2));
				ampersand = false;
			}
		}
	}

    return res;
}

overload sdef(void, Encode, String s, String *out) {
	for (size_t i = 0; i < s.len; i++) {
		switch (s.buf[i]) {
			case '&':
				String_Append(out, String("&amp;"));
				break;

			case '"':
				String_Append(out, String("&quot;"));
				break;

			case '\'':
				String_Append(out, String("&#039;"));
				break;

			case '<':
				String_Append(out, String("&lt;"));
				break;

			case '>':
				String_Append(out, String("&gt;"));
				break;

			default:
				String_Append(out, s.buf[i]);
		}
	}
}

overload sdef(String, Encode, String s) {
	String res = HeapString(s.len * HTML_Entities_GrowthFactor);
	HTML_Entities_Encode(s, &res);

	return res;
}
