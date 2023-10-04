using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.AccessControl;
using System.Web;
//using Chilkat;
using Net.Pkcs11Interop.Common;
using Net.Pkcs11Interop.HighLevelAPI;

namespace ICLab.Service
{
    public class AnotherTest
    {
        public static void Test()
        {
            // Specify the path to unmanaged PKCS#11 library provided by the cryptographic device vendor
            string pkcs11LibraryPath = @"D:\Side Project\WebSite\ICLab\ICLab\ICSCDLL.dll";

            // Create factories used by Pkcs11Interop library
            Pkcs11InteropFactories factories = new Pkcs11InteropFactories();

            // Load unmanaged PKCS#11 library
            using (IPkcs11Library pkcs11Library = factories.Pkcs11LibraryFactory.LoadPkcs11Library(factories, pkcs11LibraryPath, AppType.MultiThreaded))
            {
                // Show general information about loaded library
                ILibraryInfo libraryInfo = pkcs11Library.GetInfo();

                Console.WriteLine("Library");
                Console.WriteLine("  Manufacturer:       " + libraryInfo.ManufacturerId);
                Console.WriteLine("  Description:        " + libraryInfo.LibraryDescription);
                Console.WriteLine("  Version:            " + libraryInfo.LibraryVersion);

                // Get list of all available slots
                foreach (ISlot slot in pkcs11Library.GetSlotList(SlotsType.WithOrWithoutTokenPresent))
                {
                    // Show basic information about slot
                    ISlotInfo slotInfo = slot.GetSlotInfo();

                    Console.WriteLine();
                    Console.WriteLine("Slot");
                    Console.WriteLine("  Manufacturer:       " + slotInfo.ManufacturerId);
                    Console.WriteLine("  Description:        " + slotInfo.SlotDescription);
                    Console.WriteLine("  Token present:      " + slotInfo.SlotFlags.TokenPresent);

                    if (slotInfo.SlotFlags.TokenPresent)
                    {
                        // Show basic information about token present in the slot
                        ITokenInfo tokenInfo = slot.GetTokenInfo();

                        Console.WriteLine("Token");
                        Console.WriteLine("  Manufacturer:       " + tokenInfo.ManufacturerId);
                        Console.WriteLine("  Model:              " + tokenInfo.Model);
                        Console.WriteLine("  Serial number:      " + tokenInfo.SerialNumber);
                        Console.WriteLine("  Label:              " + tokenInfo.Label);

                        // Show list of mechanisms (algorithms) supported by the token
                        Console.WriteLine("Supported mechanisms: ");
                        foreach (CKM mechanism in slot.GetMechanismList())
                            Console.WriteLine("  " + mechanism);
                    }
                }
            }
        }


    }
}