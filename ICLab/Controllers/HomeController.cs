using ICLab.Service;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Web;
using System.Web.Http.Results;
using System.Web.Mvc;

namespace ICLab.Controllers
{
    public class HomeController : Controller
    {

        public ActionResult Index()
        {
            // CardService.GetCardInfo();

            X32Service.GetInfo();

            return View();
        }

    }
}