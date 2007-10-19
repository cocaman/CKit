/*
 * This is a test program that exercises the CKFloat.
 */

#include <iostream>
#include <stdio.h>

#include "CKFloat.h"

int main(int argc, char *argv[]) {
	try {
		/*
		 * Try the different constructors
		 */
		std::cout << "Testing the constructors:" << std::endl;
		CKFloat		c1;
		std::cout << "                          empty: " << c1 << std::endl;
		c1 = CKFloat((int) 5);
		std::cout << "                        (int) 5: " << c1 << std::endl;
		c1 = CKFloat((long) 6);
		std::cout << "                       (long) 6: " << c1 << std::endl;
		c1 = CKFloat((float) 3.14);
		std::cout << "                   (float) 3.14: " << c1 << " *" << std::endl;
		c1 = CKFloat((double) 0.12);
		std::cout << "                  (double) 0.12: " << c1 << " *" << std::endl;
		c1 = CKFloat((double) 1.414);
		std::cout << "                 (double) 1.414: " << c1 << " *" << std::endl;
		c1 = CKFloat((double) 1234567890123456.414);
		std::cout << "  (double) 1234567890123456.414: " << c1 << " *" << std::endl;
		c1 = CKFloat("22.23");
		std::cout << "                 (String) 22.23: " << c1 << std::endl;
		CKFloat		c2(c1);
		std::cout << "                   (CKFloat) c1: " << c2 << std::endl;
		c1 = CKFloat((int) -5);
		std::cout << "                       (int) -5: " << c1 << std::endl;
		c1 = CKFloat((long) -6);
		std::cout << "                      (long) -6: " << c1 << std::endl;
		c1 = CKFloat((float) -3.14);
		std::cout << "                  (float) -3.14: " << c1 << " *" << std::endl;
		c1 = CKFloat((double) -0.12);
		std::cout << "                 (double) -0.12: " << c1 << " *" << std::endl;
		c1 = CKFloat((double) -1.414);
		std::cout << "                (double) -1.414: " << c1 << " *" << std::endl;
		c1 = CKFloat("0E-5");
		std::cout << "                  (String) 0E-5: " << c1 << std::endl;
		c1 = CKFloat("0.0E-5");
		std::cout << "                (String) 0.0E-5: " << c1 << std::endl;
		c1 = CKFloat("0E+5");
		std::cout << "                  (String) 0E+5: " << c1 << std::endl;
		c1 = CKFloat("0.0E+5");
		std::cout << "                (String) 0.0E+5: " << c1 << std::endl;

		std::cout << "\nEdge-condition constructors:" << std::endl;
		c1 = CKFloat("0.12");
		std::cout << "   (String) 0.12: " << c1 << std::endl;
		c1 = CKFloat("3.14");
		std::cout << "   (String) 3.14: " << c1 << std::endl;
		c1 = CKFloat("1.414");
		std::cout << "  (String) 1.414: " << c1 << std::endl;
		c1 = CKFloat("22.4");
		std::cout << "   (String) 22.4: " << c1 << std::endl;
		c1 = CKFloat("-0.12");
		std::cout << "  (String) -0.12: " << c1 << std::endl;
		c1 = CKFloat("-3.14");
		std::cout << "  (String) -3.14: " << c1 << std::endl;
		c1 = CKFloat("-1.414");
		std::cout << " (String) -1.414: " << c1 << std::endl;
		c1 = CKFloat("-22.4");
		std::cout << "  (String) -22.4: " << c1 << std::endl;

		std::cout << "\nScientific notation constructors:" << std::endl;
		c1 = CKFloat("1.414E-3");
		std::cout << "                (String) 1.414E-3: " << c1 << " *" << std::endl;
		c1 = CKFloat("5.551115123125783E-17");
		std::cout << "   (String) 5.551115123125783E-17: " << c1 << " *" << std::endl;
		c1 = CKFloat("5.551115123125783E+17");
		std::cout << "   (String) 5.551115123125783E+17: " << c1 << " *" << std::endl;
		c1 = CKFloat("5.551115123125783E17");
		std::cout << "    (String) 5.551115123125783E17: " << c1 << " *" << std::endl;
		c1 = CKFloat("-1.414E-3");
		std::cout << "               (String) -1.414E-3: " << c1 << " *" << std::endl;
		c1 = CKFloat("-5.551115123125783E-17");
		std::cout << "  (String) -5.551115123125783E-17: " << c1 << " *" << std::endl;
		c1 = CKFloat("-5.551115123125783E+17");
		std::cout << "  (String) -5.551115123125783E+17: " << c1 << " *" << std::endl;
		c1 = CKFloat("-5.551115123125783E17");
		std::cout << "   (String) -5.551115123125783E17: " << c1 << " *" << std::endl;

		/*
		 * Try the compareTo()
		 */
		std::cout << "\nTesting the comparisons:" << std::endl;
		c1 = CKFloat("5");
		c2 = CKFloat("5.5");
		std::cout << "         (String) 5 " << (c1 > c2 ? ">" : "<") << " (String) 5.5" << std::endl;
		std::cout << "       (String) 5.5 " << (c2 > c1 ? ">" : "<") << " (String) 5\n" << std::endl;
		c1 = CKFloat("-5");
		c2 = CKFloat("-5.5");
		std::cout << "        (String) -5 " << (c1 > c2 ? ">" : "<") << " (String) -5.5" << std::endl;
		std::cout << "      (String) -5.5 " << (c2 > c1 ? ">" : "<") << " (String) -5\n" << std::endl;
		c1 = CKFloat("5");
		c2 = CKFloat("-5.5");
		std::cout << "         (String) 5 " << (c1 > c2 ? ">" : "<") << " (String) -5.5" << std::endl;
		std::cout << "      (String) -5.5 " << (c2 > c1 ? ">" : "<") << " (String) 5\n" << std::endl;
		c1 = CKFloat("-5");
		c2 = CKFloat("5.5");
		std::cout << "        (String) -5 " << (c1 > c2 ? ">" : "<") << " (String) 5.5" << std::endl;
		std::cout << "       (String) 5.5 " << (c2 > c1 ? ">" : "<") << " (String) -5\n" << std::endl;

		c1 = CKFloat("3.12");
		c2 = CKFloat("3.1");
		std::cout << "      (String) 3.12 " << (c1 > c2 ? ">" : "<") << " (String) 3.1" << std::endl;
		std::cout << "       (String) 3.1 " << (c2 > c1 ? ">" : "<") << " (String) 3.12\n" << std::endl;
		c1 = CKFloat("-3.12");
		c2 = CKFloat("-3.1");
		std::cout << "     (String) -3.12 " << (c1 > c2 ? ">" : "<") << " (String) -3.1" << std::endl;
		std::cout << "      (String) -3.1 " << (c2 > c1 ? ">" : "<") << " (String) -3.12\n" << std::endl;
		c1 = CKFloat("3.12");
		c2 = CKFloat("-3.1");
		std::cout << "      (String) 3.12 " << (c1 > c2 ? ">" : "<") << " (String) -3.1" << std::endl;
		std::cout << "      (String) -3.1 " << (c2 > c1 ? ">" : "<") << " (String) 3.12\n" << std::endl;
		c1 = CKFloat("-3.12");
		c2 = CKFloat("3.1");
		std::cout << "     (String) -3.12 " << (c1 > c2 ? ">" : "<") << " (String) 3.1" << std::endl;
		std::cout << "       (String) 3.1 " << (c2 > c1 ? ">" : "<") << " (String) -3.12\n" << std::endl;

		c1 = CKFloat("15");
		c2 = CKFloat("5.5");
		std::cout << "        (String) 15 " << (c1 > c2 ? ">" : "<") << " (String) 5.5" << std::endl;
		std::cout << "       (String) 5.5 " << (c2 > c1 ? ">" : "<") << " (String) 15\n" << std::endl;
		c1 = CKFloat("-15");
		c2 = CKFloat("-5.5");
		std::cout << "       (String) -15 " << (c1 > c2 ? ">" : "<") << " (String) -5.5" << std::endl;
		std::cout << "      (String) -5.5 " << (c2 > c1 ? ">" : "<") << " (String) -15\n" << std::endl;
		c1 = CKFloat("15");
		c2 = CKFloat("-5.5");
		std::cout << "        (String) 15 " << (c1 > c2 ? ">" : "<") << " (String) -5.5" << std::endl;
		std::cout << "      (String) -5.5 " << (c2 > c1 ? ">" : "<") << " (String) 15\n" << std::endl;
		c1 = CKFloat("-15");
		c2 = CKFloat("5.5");
		std::cout << "       (String) -15 " << (c1 > c2 ? ">" : "<") << " (String) 5.5" << std::endl;
		std::cout << "       (String) 5.5 " << (c2 > c1 ? ">" : "<") << " (String) -15\n" << std::endl;

		c1 = CKFloat("3.12");
		c2 = CKFloat("4.1");
		std::cout << "      (String) 3.12 " << (c1 > c2 ? ">" : "<") << " (String) 4.1" << std::endl;
		std::cout << "       (String) 4.1 " << (c2 > c1 ? ">" : "<") << " (String) 3.12\n" << std::endl;
		c1 = CKFloat("-3.12");
		c2 = CKFloat("-4.1");
		std::cout << "     (String) -3.12 " << (c1 > c2 ? ">" : "<") << " (String) -4.1" << std::endl;
		std::cout << "      (String) -4.1 " << (c2 > c1 ? ">" : "<") << " (String) -3.12\n" << std::endl;
		c1 = CKFloat("3.12");
		c2 = CKFloat("-4.1");
		std::cout << "      (String) 3.12 " << (c1 > c2 ? ">" : "<") << " (String) -4.1" << std::endl;
		std::cout << "      (String) -4.1 " << (c2 > c1 ? ">" : "<") << " (String) 3.12\n" << std::endl;
		c1 = CKFloat("-3.12");
		c2 = CKFloat("4.1");
		std::cout << "     (String) -3.12 " << (c1 > c2 ? ">" : "<") << " (String) 4.1" << std::endl;
		std::cout << "       (String) 4.1 " << (c2 > c1 ? ">" : "<") << " (String) -3.12\n" << std::endl;

		c1 = CKFloat("4.12");
		c2 = CKFloat("4.2");
		std::cout << "      (String) 4.12 " << (c1 > c2 ? ">" : "<") << " (String) 4.2" << std::endl;
		std::cout << "       (String) 4.2 " << (c2 > c1 ? ">" : "<") << " (String) 4.12\n" << std::endl;
		c1 = CKFloat("-4.12");
		c2 = CKFloat("-4.2");
		std::cout << "     (String) -4.12 " << (c1 > c2 ? ">" : "<") << " (String) -4.2" << std::endl;
		std::cout << "      (String) -4.2 " << (c2 > c1 ? ">" : "<") << " (String) -4.12\n" << std::endl;
		c1 = CKFloat("4.12");
		c2 = CKFloat("-4.2");
		std::cout << "      (String) 4.12 " << (c1 > c2 ? ">" : "<") << " (String) -4.2" << std::endl;
		std::cout << "      (String) -4.2 " << (c2 > c1 ? ">" : "<") << " (String) 4.12\n" << std::endl;
		c1 = CKFloat("-4.12");
		c2 = CKFloat("4.2");
		std::cout << "     (String) -4.12 " << (c1 > c2 ? ">" : "<") << " (String) 4.2" << std::endl;
		std::cout << "       (String) 4.2 " << (c2 > c1 ? ">" : "<") << " (String) -4.12" << std::endl;

		/*
		 * Try the output values
		 */
		std::cout << "\nTesting the output values:" << std::endl;
		c1 = CKFloat((int) 5);
		std::cout << "         (int) 5: " << c1 << " = " << c1.intValue() << std::endl;
		c1 = CKFloat((long) 6);
		std::cout << "        (long) 6: " << c1 << " = " << c1.longValue() << std::endl;
		c1 = CKFloat((float) 3.14);
		std::cout << "    (float) 3.14: " << c1 << " *" << " = " << c1.floatValue() << std::endl;
		c1 = CKFloat((double) 1.414);
		std::cout << "  (double) 1.414: " << c1 << " *" << " = " << c1.doubleValue() << std::endl;

		/*
		 * Try the shifting (powers of ten)
		 */
		std::cout << "\nTesting the powers of ten:" << std::endl;
		c1 = CKFloat("1234.5678");
		std::cout << "    (String) 1234.5678: " << c1 << " = " << c1.doubleValue() << std::endl;
		c1.multiplyBy10(1);
		std::cout << " *10                  : " << c1 << " = " << c1.doubleValue() << std::endl;
		c1.multiplyBy10(-2);
		std::cout << " *10^-2               : " << c1 << " = " << c1.doubleValue() << std::endl;
		c1.multiplyBy10(3);
		std::cout << " *10^3                : " << c1 << " = " << c1.doubleValue() << std::endl;
		c1.multiplyBy10(-4);
		std::cout << " *10^-4               : " << c1 << " = " << c1.doubleValue() << std::endl;
		c1.multiplyBy10(5);
		std::cout << " *10^5                : " << c1 << " = " << c1.doubleValue() << std::endl;
		c1.multiplyBy10(-6);
		std::cout << " *10^-6               : " << c1 << " = " << c1.doubleValue() << std::endl;
		c1.multiplyBy10(7);
		std::cout << " *10^7                : " << c1 << " = " << c1.doubleValue() << std::endl;
		c1.multiplyBy10(-8);
		std::cout << " *10^-8               : " << c1 << " = " << c1.doubleValue() << std::endl;
		c1.multiplyBy10(9);
		std::cout << " *10^9                : " << c1 << " = " << c1.doubleValue() << std::endl;
		c1.multiplyBy10(-10);
		std::cout << " *10^-10              : " << c1 << " = " << c1.doubleValue() << std::endl;
		c1.multiplyBy10(11);
		std::cout << " *10^11               : " << c1 << " = " << c1.doubleValue() << std::endl;
		c1.multiplyBy10(-12);
		std::cout << " *10^-12              : " << c1 << " = " << c1.doubleValue() << std::endl;
		c1.multiplyBy10(13);
		std::cout << " *10^13               : " << c1 << " = " << c1.doubleValue() << std::endl;

		/*
		 * Try some adds
		 */
		std::cout << "\nTesting the add:" << std::endl;
		c1 = CKFloat("0.12");
		std::cout << "         (String) 0.12: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("0.0012");
		c1.add(c2);
		std::cout << " +     (String) 0.0012: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("0.1111");
		c1.add(c2);
		std::cout << " +     (String) 0.1111: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("0.1118");
		c1.add(c2);
		std::cout << " +     (String) 0.1118: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("0.6559");
		c1.add(c2);
		std::cout << " +     (String) 0.6559: " << c1 << " = " << c1.doubleValue() << std::endl;

		std::cout << "\nTesting the add (with negatives):" << std::endl;
		c1 = CKFloat("-0.12");
		std::cout << "        (String) -0.12: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("-0.0012");
		c1.add(c2);
		std::cout << " +    (String) -0.0012: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("-0.1111");
		c1.add(c2);
		std::cout << " +    (String) -0.1111: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("-0.1118");
		c1.add(c2);
		std::cout << " +    (String) -0.1118: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("-0.6559");
		c1.add(c2);
		std::cout << " +    (String) -0.6559: " << c1 << " = " << c1.doubleValue() << std::endl;

		std::cout << "\nTesting the add (bigger numbers):" << std::endl;
		c1 = CKFloat("21.12");
		std::cout << "        (String) 21.12: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("5.0012");
		c1.add(c2);
		std::cout << " +     (String) 5.0012: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("3.1111");
		c1.add(c2);
		std::cout << " +     (String) 3.1111: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("2.1118");
		c1.add(c2);
		std::cout << " +     (String) 2.1118: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("68.6559");
		c1.add(c2);
		std::cout << " +    (String) 68.6559: " << c1 << " = " << c1.doubleValue() << std::endl;

		std::cout << "\nTesting the subtract:" << std::endl;
		c1 = CKFloat("100");
		std::cout << "          (String) 100: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("0.0012");
		c1.subtract(c2);
		std::cout << " -     (String) 0.0012: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("0.1111");
		c1.subtract(c2);
		std::cout << " -     (String) 0.1111: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("0.1118");
		c1.subtract(c2);
		std::cout << " -     (String) 0.1118: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("0.6559");
		c1.subtract(c2);
		std::cout << " -     (String) 0.6559: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("99.12");
		c1.subtract(c2);
		std::cout << " -      (String) 99.12: " << c1 << " = " << c1.doubleValue() << std::endl;

		std::cout << "\nTesting the multiply:" << std::endl;
		c1 = CKFloat("4");
		std::cout << "          (String) 4: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("0.25");
		c1.multiply(c2);
		std::cout << " *     (String) 0.25: " << c1 << " = " << c1.doubleValue() << std::endl;

		std::cout << "" << std::endl;
		c1 = CKFloat("12.332");
		std::cout << "     (String) 12.332: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("5.32");
		c1.multiply(c2);
		std::cout << " *     (String) 5.32: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("-1.31");
		c1.multiply(c2);
		std::cout << " *    (String) -1.31: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("-2.002");
		c1.multiply(c2);
		std::cout << " *   (String) -2.002: " << c1 << " = " << c1.doubleValue() << std::endl;

		std::cout << "" << std::endl;
		c1 = CKFloat("1");
		std::cout << "        (String) 1: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("12.5");
		c1.multiply(c2);
		std::cout << " *   (String) 12.5: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("1.2");
		c1.multiply(c2);
		std::cout << " *    (String) 1.2: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("-0.55");
		c1.multiply(c2);
		std::cout << " *  (String) -0.55: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("3.4");
		c1.multiply(c2);
		std::cout << " *    (String) 3.4: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("0.75");
		c1.multiply(c2);
		std::cout << " *   (String) 0.75: " << c1 << " = " << c1.doubleValue() << std::endl;

		c1 = CKFloat("5.551115123125783E-17");
		std::cout << "\n   (String) 5.551115123125783E-17: " << c1 << std::endl;
		c2 = CKFloat("1E17");
		c1.multiply(c2);
		std::cout << " *                  (String) 1E17: " << c1 << " = " << c1.doubleValue() << std::endl;

		std::cout << "\nTesting the divide:" << std::endl;
		c1 = CKFloat("4");
		std::cout << "          (String) 4: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("0.25");
		c1.divide(c2);
		std::cout << " /     (String) 0.25: " << c1 << " = " << c1.doubleValue() << std::endl;

		std::cout << "" << std::endl;
		c1 = CKFloat("-4");
		std::cout << "         (String) -4: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("0.25");
		c1.divide(c2);
		std::cout << " /     (String) 0.25: " << c1 << " = " << c1.doubleValue() << std::endl;

		std::cout << "" << std::endl;
		c1 = CKFloat("4");
		std::cout << "          (String) 4: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("-0.25");
		c1.divide(c2);
		std::cout << " /    (String) -0.25: " << c1 << " = " << c1.doubleValue() << std::endl;

		std::cout << "" << std::endl;
		c1 = CKFloat("-4");
		std::cout << "         (String) -4: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("-0.25");
		c1.divide(c2);
		std::cout << " /    (String) -0.25: " << c1 << " = " << c1.doubleValue() << std::endl;

		std::cout << "" << std::endl;
		c1 = CKFloat("5");
		std::cout << "          (String) 5: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("2");
		c1.divide(c2);
		std::cout << " /        (String) 2: " << c1 << " = " << c1.doubleValue() << std::endl;

		std::cout << "" << std::endl;
		c1 = CKFloat("10");
		std::cout << "         (String) 10: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("3");
		c1.divide(c2);
		std::cout << " /        (String) 3: " << c1 << " = " << c1.doubleValue() << std::endl;

		std::cout << "" << std::endl;
		c1 = CKFloat("1");
		std::cout << "          (String) 1: " << c1 << " = " << c1.doubleValue() << std::endl;
		c2 = CKFloat("3");
		c1.divide(c2);
		std::cout << " /        (String) 3: " << c1 << " = " << c1.doubleValue() << std::endl;
	} catch (CKException & cke) {
		std::cout << "Exception: " << cke.getMessage() << std::endl;
	} catch (...) {
		std::cout << "unknown exception" << std::endl;
	}
	return(0);
}
