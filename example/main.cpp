/*******************************************************************************
 * QRESTServer a lean and mean Qt/C++ based REST server                        *
 *                                                                             *
 * Copyright 2018 by Targoman Intelligent Processing Co Pjc.<http://tip.co.ir> *
 *                                                                             *
 *                                                                             *
 * QRESTServer is free software: you can redistribute it and/or modify         *
 * it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE as published by *
 * the Free Software Foundation, either version 3 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * QRESTServer is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU AFFERO GENERAL PUBLIC LICENSE for more details.                         *
 * You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE    *
 * along with QRESTServer. If not, see <http://www.gnu.org/licenses/>.         *
 *                                                                             *
 *******************************************************************************/
/**
 * @author S.Mehran M.Ziabary <ziabary@targoman.com>
 */

#include <iostream>
#include <QDebug>
#include <QCoreApplication>
#include "libTargomanCommon/exTargomanBase.h"
#include "QHttp/QRESTServer.h"
#include "libTargomanCommon/Logger.h"
#include "SampleAPI.h"

using namespace QHttp;

int main(int _argc, char *_argv[])
{
    Q_UNUSED(_argc)
    Q_UNUSED(_argv)

    try{
        QCoreApplication App(_argc, _argv);

        Targoman::Common::TARGOMAN_IO_SETTINGS.setFull();
        //Targoman::Common::Logger::instance().init();

        Sample1::SampleAPI::instance().init();

        std::cout<<qPrintable(RESTServer::registeredAPIs(true, true).join("\n"))<<std::endl;
        RESTServer::configure (RESTServer::stuConfig("/a", "v2", 9009, true));
        RESTServer::start();

       App.exec();
    }catch(Targoman::Common::exTargomanBase &e){
        qDebug()<<e.what();
        return 1;
    }
    return 0;
}


