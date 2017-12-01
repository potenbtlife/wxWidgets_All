#include "CRTData.h"
#include <direct.h>
#include <errno.h>
#include <wx/dir.h>

//根据域名来获取ip地址
int Domain2Ip(const char * url, char* ipaddr)
{
    if(url == ""){
        return -1;
    }
    HOSTENT * iphost;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    if((iphost = gethostbyname(url)) != NULL)
    {
        if(iphost->h_addr_list[0]) {
            memcpy(ipaddr, inet_ntoa(*((struct in_addr *)iphost->h_addr_list[0])), 20);
        }

    }else {
        wxLogError("Domain2Ip gethostbyname failed! errcode[%d], url[%s]\n", WSAGetLastError(), url);
        WSACleanup();
        return -1;
    }

    WSACleanup();

    return 1;
}

CRTDataProxy::CRTDataProxy(char* proxyIP, int proxyPort,const char* username, const char* pwd)
    : m_proxyIP(proxyIP), m_proxyPort(proxyPort), m_proxyUserName(username), m_proxyPassword(pwd){
    
}

//return 1:正常取出，还有； 0: 正常取完；-1：异常错误
int CRTData::GetAllStockPrice(std::map<std::string, RTInfo>& mapOut, int& curpos){

    vector<string> vecAllIDName;
    getAllStockIdName(vecAllIDName);

    std::string allCode;
    for (; curpos<vecAllIDName.size(); ++curpos) {

        int index = vecAllIDName[curpos].find("_",0);
        if (index == string::npos) {
            continue;
        }
        string stock_id = vecAllIDName[curpos].substr(0,index).c_str();
        
        /*if(stock_id.find("00055") != string::npos){
            int test =1;
        }*/
        allCode += genUrlRequestCode(stock_id) + ",";
        
        if((curpos+1)%100 == 0){
            if( GetRtDataFromHttp(allCode, mapOut) < 0 ){
                return -1;
            }else{
                ++curpos;
                return 1;
            }
        }
    }

    

    return 0;
}

int CRTData::RefreshRtData(){
    m_mOutData.clear();

    std::string allCode;
    for (vector<std::string>::iterator iter = m_vCode.begin(); iter != m_vCode.end(); ++iter) {
        allCode += trim(genUrlRequestCode(*iter)) + ",";
    }

    return GetRtDataFromHttp(allCode, m_mOutData);
}

int CRTData::GetRtDataFromHttp(string& allCode, std::map<std::string, RTInfo>& mOutData){

    std::string tmpAllData; 
    string url="http://hq.sinajs.cn/list="+allCode;
    if( GetHttpData(url, tmpAllData) < 0 ) {
        return -1;
    }
    string::size_type pos = tmpAllData.find("var hq_str_");
    std::string varData;
    if(pos != string::npos){
        varData = tmpAllData.substr(pos); //截掉http头
    }

    vector<std::string> vTmp1;
    splitString(varData, vTmp1, ";", false);
    for (vector<std::string>::iterator iter = vTmp1.begin(); iter != vTmp1.end(); ++iter) {

        //格式：var hq_str_hk01247="MIKO INTL,米格国际控股,0.640,0.630,0.640,0.610,0.630,0.000,0.000,0.610,0.630,247620,402000,3.520,5.079,1.280,0.600,2015/09/25,16:01"
        //var hq_str_sz002236="大华股份,37.92,37.89,36.90,38.10,36.85,36.90,36.94,9061407,337714523.00,42800,36.90,10300,36.89,26900,36.88,26420,36.87,8000,36.86,300,36.94,1700,36.95,3200,36.96,300,36.97,36300,36.98,2015-12-31,15:34:27,00";
        int startPos = iter->find("hq_str_");
        if(std::string::npos == startPos) continue; //找不到的话跳过
        startPos += 7;
        std::string tmpCode(*iter, startPos, iter->rfind("=")-startPos);//取id

        startPos = iter->find("\"") + 1;
        std::string stockInfo(*iter, startPos, iter->rfind("\"")-startPos);//取中间数据

        vector<std::string> vtmpTest;
        splitString(stockInfo, vtmpTest, ",", false);
        RTInfo tmpRTInfo;
        if (tmpCode.substr(0,2)=="hk" && vtmpTest.size()> 6 && vtmpTest.size() <= 19) //19列的为hk的
        {
            tmpRTInfo.name = vtmpTest[1];
            sscanf(vtmpTest[6].c_str(), "%f", &tmpRTInfo.price);
            mOutData.insert(pair<std::string, RTInfo>(reverseUrlCode(tmpCode), tmpRTInfo));

        }else if(vtmpTest.size()> 6 && vtmpTest.size() <= 33){ //沪深
            tmpRTInfo.name = vtmpTest[0];
            sscanf(vtmpTest[3].c_str(), "%f", &tmpRTInfo.price);
            mOutData.insert(pair<std::string, RTInfo>(reverseUrlCode(tmpCode), tmpRTInfo));

        }else{
            continue;
        }

    }


    return 1;
}

//get balance report, param[in] reportType : 0: 年报； 1：中报; param[out] vecOneAllBalance
int CRTData::GetBalanceReport(string stock_id, int reportType, vector<BalanceData>& vecOneAllBalance){

    string outStr;
    //http://stock.finance.sina.com.cn/hkstock/api/jsonp.php/var%20tableData%20=%20/FinanceStatusService.getBalanceSheetForjs?symbol=03828&balanceSheet=zero
    string url = "http://stock.finance.sina.com.cn/hkstock/api/jsonp.php/var%20tableData%20=%20/FinanceStatusService.getBalanceSheetForjs?symbol=";
    url += stock_id;
    if (reportType ==0) {
        url += "&balanceSheet=zero";
    }else{
        url += "&balanceSheet=1";
    }
    
    if( GetHttpData(url,outStr) < 0 ){
        return -1;
    }
    
    char* beginStr ="([[";
    char* endStr="]])";
    size_t beginPos = outStr.find(beginStr);
    size_t endPos = outStr.find(endStr);
    if(beginPos == string::npos || endPos == string::npos){
        return -1;
    }
    string outstr2 = outStr.substr(beginPos+3, endPos-beginPos-3);
    
    vector<std::string> vec1;
    splitString(outstr2, vec1, "],[", false, true);
    
    BalanceData tmpBalance;
    for (int i=0; i<vec1.size(); ++i) {
        vector<std::string> vec2;
        splitString(vec1[i], vec2, ",", false);
        
        if ( vec2.size() == 27 ) {
            tmpBalance.stock_id = stock_id;
            tmpBalance.report_time = trim(vec2[0], "\"");
            tmpBalance.report_type=trim(vec2[1], "\"");
            tmpBalance.static_asset = atof(trim(vec2[2],"\"").c_str());
            tmpBalance.flow_asset = atof(trim(vec2[3],"\"").c_str());
            tmpBalance.flow_debt = atof(trim(vec2[4],"\"").c_str());
            tmpBalance.netflow_asset = atof(trim(vec2[5],"\"").c_str());
            tmpBalance.static_debt = atof(trim(vec2[6],"\"").c_str());
            tmpBalance.fewholder_asset = atof(trim(vec2[7],"\"").c_str());
            tmpBalance.net_asset = atof(trim(vec2[8],"\"").c_str());
            tmpBalance.guben = atof(trim(vec2[9],"\"").c_str());
            tmpBalance.chubei = atof(trim(vec2[10],"\"").c_str());
            tmpBalance.holder_asset = atof(trim(vec2[11],"\"").c_str());
            tmpBalance.invisible_asset = atof(trim(vec2[12],"\"").c_str());
            tmpBalance.building_device = atof(trim(vec2[13],"\"").c_str());
            tmpBalance.childcort_asset = atof(trim(vec2[14],"\"").c_str());
            tmpBalance.sibing_asset = atof(trim(vec2[15],"\"").c_str());
            tmpBalance.other_invest = atof(trim(vec2[16],"\"").c_str());
            tmpBalance.yingshou = atof(trim(vec2[17],"\"").c_str());
            tmpBalance.cunhuo = atof(trim(vec2[18],"\"").c_str());
            tmpBalance.cash = atof(trim(vec2[19],"\"").c_str());
            tmpBalance.yingfu = atof(trim(vec2[20],"\"").c_str());
            tmpBalance.flow_bankdept = atof(trim(vec2[21],"\"").c_str());
            tmpBalance.static_bankdept = atof(trim(vec2[22],"\"").c_str());
            tmpBalance.all_asset = atof(trim(vec2[23],"\"").c_str());
            tmpBalance.all_dept = atof(trim(vec2[24],"\"").c_str());
            tmpBalance.stock_share = atof(trim(vec2[25],"\"").c_str());
            tmpBalance.bizhong =trim(vec2[26], "\"");

        }else if(vec2.size() == 32) {
            tmpBalance.stock_id = stock_id;
            tmpBalance.report_time = trim(vec2[0], "\"");
            tmpBalance.report_type=trim(vec2[1], "\"");
            tmpBalance.static_asset = atof(trim(vec2[11],"\"").c_str());
            tmpBalance.flow_asset = atof(trim(vec2[2],"\"").c_str());
            tmpBalance.flow_debt = atof(trim(vec2[16],"\"").c_str());
            tmpBalance.netflow_asset = 0; //净流动资产/(负债)
            tmpBalance.static_debt = atof(trim(vec2[18],"\"").c_str());
            tmpBalance.fewholder_asset = atof(trim(vec2[24],"\"").c_str());
            tmpBalance.net_asset = atof(trim(vec2[23],"\"").c_str());
            tmpBalance.guben = atof(trim(vec2[21],"\"").c_str());
            tmpBalance.chubei = atof(trim(vec2[22],"\"").c_str());
            tmpBalance.holder_asset = atof(trim(vec2[23],"\"").c_str());
            tmpBalance.invisible_asset = atof(trim(vec2[10],"\"").c_str());
            tmpBalance.building_device = atof(trim(vec2[11],"\"").c_str());
            tmpBalance.childcort_asset = 0;//附属公司权益(非流动资产)	
            tmpBalance.sibing_asset = atof(trim(vec2[8],"\"").c_str());
            tmpBalance.other_invest = atof(trim(vec2[9],"\"").c_str());
            tmpBalance.yingshou = atof(trim(vec2[6],"\"").c_str());
            tmpBalance.cunhuo = 0;//存货
            tmpBalance.cash = atof(trim(vec2[2],"\"").c_str());
            tmpBalance.yingfu = 0; //应付帐款(流动负债)
            tmpBalance.flow_bankdept = 0; //银行贷款(流动负债)	
            tmpBalance.static_bankdept = 0; //非流动银行贷款	
            tmpBalance.all_asset = atof(trim(vec2[13],"\"").c_str());
            tmpBalance.all_dept = atof(trim(vec2[19],"\"").c_str());
            tmpBalance.stock_share = atof(trim(vec2[27],"\"").c_str());
            tmpBalance.bizhong =trim(vec2[31], "\"");
        }else{
            wxLogError(wxT("column[%d] is undefined!"), vec2.size());
            return -1;
        }


        vecOneAllBalance.push_back(tmpBalance);
    }

    return 1;
}

//get sun yi report, param[in] reportType : 0: 年报； 1：中报; param[out] vecOneAllSunYi
int CRTData::GetSunYiReport(string stock_id, int reportType,vector<SunYiData>& vecOneAllSunYi){

    string outStr;
    //http://stock.finance.sina.com.cn/hkstock/api/jsonp.php/var%20tableData%20=%20/FinanceStatusService.getFinanceStatusForjs?symbol=03828&financeStatus=zero
    string url = "http://stock.finance.sina.com.cn/hkstock/api/jsonp.php/var%20tableData%20=%20/FinanceStatusService.getFinanceStatusForjs?symbol=";
    url += stock_id;
    if (reportType ==0) {
        url += "&financeStatus=zero";
    }else{
        url += "&financeStatus=1";
    }

    if( GetHttpData(url,outStr) < 0 ){
        return -1;
    }

    char* beginStr ="([[";
    char* endStr="]])";
    size_t beginPos = outStr.find(beginStr);
    size_t endPos = outStr.find(endStr);
    if(beginPos == string::npos || endPos == string::npos){
        return -1;
    }
    string outstr2 = outStr.substr(beginPos+3, endPos-beginPos-3);

    vector<std::string> vec1;
    splitString(outstr2, vec1, "],[", false, true);

    SunYiData tmpSunYiData;
    for (int i=0; i<vec1.size(); ++i) {
        vector<std::string> vec2;
        splitString(vec1[i], vec2, ",", false);

        if( vec2.size()<20 ){
            return -1;
        }

        if(vec2.size() == 20 ){ //22列的赋值
            tmpSunYiData.stock_id = stock_id;
            tmpSunYiData.report_time = trim(vec2[0], "\"");
            tmpSunYiData.report_type = trim(vec2[1], "\"");
            tmpSunYiData.yingyee = atof(trim(vec2[2],"\"").c_str());
            tmpSunYiData.untax_profit = atof(trim(vec2[10],"\"").c_str());
            tmpSunYiData.tax = atof(trim(vec2[11],"\"").c_str());
            tmpSunYiData.tax_profit = atof(trim(vec2[12],"\"").c_str());
            tmpSunYiData.fewholder_profit = atof(trim(vec2[13],"\"").c_str());
            tmpSunYiData.holder_profit = atof(trim(vec2[14],"\"").c_str());
            tmpSunYiData.dividend = atof(trim(vec2[15],"\"").c_str());
            tmpSunYiData.net_profit = atof(trim(vec2[16],"\"").c_str());
            tmpSunYiData.basic_one_profit = atof(trim(vec2[17],"\"").c_str());
            tmpSunYiData.tanxiao_one_profit = atof(trim(vec2[18],"\"").c_str());
            tmpSunYiData.ont_dividend = 0; //每股股息
            tmpSunYiData.cost = atof(trim(vec2[7],"\"").c_str());
            tmpSunYiData.depreciation = 0;//折旧
            tmpSunYiData.sell_fee = atof(trim(vec2[7],"\"").c_str());
            tmpSunYiData.admin_fee = 0;//行政费用
            tmpSunYiData.lixi_fee = atof(trim(vec2[3],"\"").c_str());
            tmpSunYiData.gross_profit = atof(trim(vec2[4],"\"").c_str());
            tmpSunYiData.jingying_profit = atof(trim(vec2[10],"\"").c_str());
            tmpSunYiData.othercorp_profit = 0; //应占联营公司盈利
            tmpSunYiData.bizhong =trim(vec2[19],"\"");

        } else if(vec2.size() >= 22 ){ //22列的赋值
            tmpSunYiData.stock_id = stock_id;
            tmpSunYiData.report_time = trim(vec2[0], "\"");
            tmpSunYiData.report_type = trim(vec2[1], "\"");
            tmpSunYiData.yingyee = atof(trim(vec2[2],"\"").c_str());
            tmpSunYiData.untax_profit = atof(trim(vec2[3],"\"").c_str());
            tmpSunYiData.tax = atof(trim(vec2[4],"\"").c_str());
            tmpSunYiData.tax_profit = atof(trim(vec2[5],"\"").c_str());
            tmpSunYiData.fewholder_profit = atof(trim(vec2[6],"\"").c_str());
            tmpSunYiData.holder_profit = atof(trim(vec2[7],"\"").c_str());
            tmpSunYiData.dividend = atof(trim(vec2[8],"\"").c_str());
            tmpSunYiData.net_profit = atof(trim(vec2[9],"\"").c_str());
            tmpSunYiData.basic_one_profit = atof(trim(vec2[10],"\"").c_str());
            tmpSunYiData.tanxiao_one_profit = atof(trim(vec2[11],"\"").c_str());
            tmpSunYiData.ont_dividend = atof(trim(vec2[12],"\"").c_str());
            tmpSunYiData.cost = atof(trim(vec2[13],"\"").c_str());
            tmpSunYiData.depreciation = atof(trim(vec2[14],"\"").c_str());
            tmpSunYiData.sell_fee = atof(trim(vec2[15],"\"").c_str());
            tmpSunYiData.admin_fee = atof(trim(vec2[16],"\"").c_str());
            tmpSunYiData.lixi_fee = atof(trim(vec2[17],"\"").c_str());
            tmpSunYiData.gross_profit = atof(trim(vec2[18],"\"").c_str());
            tmpSunYiData.jingying_profit = atof(trim(vec2[19],"\"").c_str());
            tmpSunYiData.othercorp_profit = atof(trim(vec2[20],"\"").c_str());
            tmpSunYiData.bizhong =trim(vec2[21],"\"");
        }

        vecOneAllSunYi.push_back(tmpSunYiData);
    }

    return 1;
}

//get cash flow report, param[in] reportType : 0: 年报； 1：中报; param[out] vecOneAllCashFlow
int CRTData::GetCashFlowReport(string stock_id, int reportType, vector<CashFlowData>& vecOneAllCashFlow){

    string outStr;
    //http://stock.finance.sina.com.cn/hkstock/api/jsonp.php/var%20tableData%20=%20/FinanceStatusService.getCashFlowForjs?symbol=03828&cashFlow=zero
    string url = "http://stock.finance.sina.com.cn/hkstock/api/jsonp.php/var%20tableData%20=%20/FinanceStatusService.getCashFlowForjs?symbol=";
    url += stock_id;
    if (reportType ==0) {
        url += "&cashFlow=zero";
    }else{
        url += "&cashFlow=1";
    }

    int ret = GetHttpData(url,outStr);
    if( ret < 0 ){
        return ret;
    }

    char* beginStr ="([[";
    char* endStr="]])";
    size_t beginPos = outStr.find(beginStr);
    size_t endPos = outStr.find(endStr);
    if(beginPos == string::npos || endPos == string::npos){
        return -1;
    }
    string outstr2 = outStr.substr(beginPos+3, endPos-beginPos-3);

    vector<std::string> vec1;
    splitString(outstr2, vec1, "],[", false, true);

    CashFlowData tmpCashFlowData;
    for (int i=0; i<vec1.size(); ++i) {
        vector<std::string> vec2;
        splitString(vec1[i], vec2, ",", false);

        if( vec2.size()<11 ){
            return -1;
        }
        tmpCashFlowData.stock_id = stock_id;   
        tmpCashFlowData.report_time =trim(vec2[0], "\"");
        tmpCashFlowData.report_type = trim(vec2[1], "\"");
        tmpCashFlowData.jingying_net = atof(trim(vec2[2], "\"").c_str());         
        tmpCashFlowData.invest_net = atof(trim(vec2[3],"\"").c_str()); 
        tmpCashFlowData.rongzi_net = atof(trim(vec2[4],"\"").c_str()); 
        tmpCashFlowData.cash_add = atof(trim(vec2[5],"\"").c_str()); 
        tmpCashFlowData.cash_begin = atof(trim(vec2[6],"\"").c_str()); 
        tmpCashFlowData.cash_end = atof(trim(vec2[7],"\"").c_str()); 
        tmpCashFlowData.waihui_profit = atof(trim(vec2[8],"\"").c_str()); 
        tmpCashFlowData.buy_device = atof(trim(vec2[9],"\"").c_str()); 
        tmpCashFlowData.bizhong = trim(vec2[10],"\""); 

        vecOneAllCashFlow.push_back(tmpCashFlowData);
    }

    return 1;
}

int CRTDataProxy::GetHttpData(string url, string& allGetData, FILE* flstm){

    string::size_type hostBegin = url.find("//", 0);
    string::size_type hostEnd = url.find("/", hostBegin+2);
    string host = url.substr(hostBegin+2, hostEnd-hostBegin-2);

    resetSocket(host);

    string httpHead="GET "+trim(url)+" HTTP/1.1\r\n";
    //httpHead += "Host: stock.finance.sina.com.cn\r\n";
    httpHead += "Host: "+host+"\r\n";
    httpHead += "Proxy-Connection: keep-alive\r\n";
    httpHead += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n";
    httpHead += "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.118 Safari/537.36\r\n";
    //httpHead += "Accept-Encoding: gzip, deflate, sdch\r\n"; 
	httpHead += "Accept-Encoding: deflate, sdch\r\n";
    httpHead += "Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\n\r\n";

    int sRet=-1;
    sRet = send(m_Socket, httpHead.c_str(), httpHead.length(), 0);
    if(SOCKET_ERROR == sRet) { //发送数据失败后，断开连接，再重新连接
        wxLogError("send packet faild! errcode[%d], httpHead[%s]",WSAGetLastError(), httpHead.c_str());
        return -1;
    }
    
    int httpCode = 0;
    int res = Recv(m_Socket, allGetData, httpCode);
    if(0 > res) {
        wxLogError("Recv error! allGetData[%s]", allGetData);
        return res-10;
    }

    if (httpCode == 200) //成功返回
    { 
        return 1;
    }

    /*char *t = "c00294700:#cdq860825"; 
    int i = 0; 
    int j = strlen(t); */
    string tmpStr = m_proxyUserName +":"+ m_proxyPassword;
    char *enc = base64_encode(tmpStr.c_str(), tmpStr.size()); 

    //发送第一次认证消息
    string AuthHead="GET "+trim(url)+" HTTP/1.1\r\n";
    AuthHead += "Host: "+host+"\r\n";
    AuthHead += "Proxy-Connection: keep-alive\r\n";
    //AuthHead += "Proxy-Authorization: NTLM TlRMTVNTUAABAAAAB4IIogAAAAAAAAAAAAAAAAAAAAAGAbEdAAAADw==\r\n";
    AuthHead += "Proxy-Authorization: Basic ";
    AuthHead += enc;
    AuthHead += "\r\n";
    AuthHead += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n";
    AuthHead += "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.118 Safari/537.36\r\n";
    AuthHead += "Accept-Encoding: gzip, deflate, sdch\r\n";
    AuthHead += "Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\n\r\n";

    sRet = send(m_Socket, AuthHead.c_str(), AuthHead.length(), 0);
    if(SOCKET_ERROR == sRet) {
        wxLogError("send error! AuthHead[%s]", AuthHead);
        return -3;
    }


    if(0 > Recv(m_Socket, allGetData, httpCode, flstm)) { //此处读数据，没读全，需要调试
        printf("Recv error!\r\n");
        return -4;
    }

    if (httpCode != 200) {
        //wxLogError("http code is not 200! allGetData[%s]", allGetData);
        wxLogError("http code is not 200! url[%s]", url.c_str());
        return -5;
    }

    return 1;
}

//获取socket套接口中所有接收缓冲区的数据，返回获取到的字节数
//param[out]code:返回码
int CRTData::Recv(int sockfd, string& allGetData, int& code, FILE* flstm) {
    allGetData = "";
    char recvBuf[131072]; //1k
    bool needRecv = true;
    int gotDataCount = 0;
    int hasHead = true;//标识此数据是否包含http头部，也就是第一个包

    while (needRecv) {
        memset(recvBuf, 0, sizeof(recvBuf));
        
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(sockfd, &readSet);
        timeval tv;
        tv.tv_sec =0;
        tv.tv_usec = 100000; //单位为微秒，100毫秒
        
        int ret = select(sockfd+1, &readSet, NULL, NULL, &tv); //如果不用select，在多线程模式下,会访问越界，故用非阻塞模式
        if(  ret < 0 ){
            wxLogError("Recv select error! errcode[%d]", WSAGetLastError());
            return -1;
        }else if( ret == 0){ //0 为timeout
            //Sleep(200); //参数为毫秒
            //wxThread::Sleep(200);
            continue;
        }
        
        int rRet = recv(sockfd, recvBuf, sizeof(recvBuf), 0);
        if(SOCKET_ERROR == rRet) {
            wxLogError("Recv recv error! errcode[%d]", WSAGetLastError());
            return -1;
        }else if(0 == rRet){
            return gotDataCount;
        }

        static int headLength = 0;
        static int packageLength = 0;
        
        if(hasHead) {
            if(-1 == checkHttpHead(recvBuf, code)){
                wxLogError("check http head failed! recvBuf[%s]", recvBuf);
                return -2;
            }

            headLength = getHttpHeadLength(recvBuf);
            if(-1 == headLength) {
                wxLogError("getHttpHeadLength failed! recvBuf[%s]", recvBuf);
                return -3;
            }
            
        }

        if(flstm != NULL && code == 200){ //如果文件流不为空，直接写入文件；否则放到出参allGetData中
            if(hasHead){
                size_t res = fwrite(recvBuf+headLength, sizeof(char), rRet-headLength, flstm);
            }else{
                size_t res = fwrite(recvBuf, sizeof(char), rRet, flstm);
            }
        }

        //printf("rRet=[%d], headLength[%d], packageLength[%d]\r\n", rRet, headLength, packageLength);

        allGetData += string(recvBuf);
        
        gotDataCount += rRet;

        packageLength = getHttpDataLength(allGetData.c_str(), allGetData.size());
        if(-1 == packageLength) {
            wxLogError("getHttpDataLength failed! allGetData[%s]", allGetData);
            return -4;
        }

        if(gotDataCount >= (headLength + packageLength)) { //读到的字节数大于等于http的数据长度，不需要再读了。
            needRecv = false;
        }

        //if( isChunkEndPackage(recvBuf) ){
        //    needRecv = false;
        //}

        hasHead = false;
    }

    return gotDataCount;
}



//return: 0:是；-1：否; param[out]:code 返回码
int checkHttpHead(const char* src, int& code) {

    if(NULL == strstr(src, "HTTP/1")){ //may by 1.1 or 1.0
        wxLogError("checkHttpHead cann't not find head: HTTP/1.1! src[%s]", src);
        return -1;
    }

    code=0;
    sscanf(src, "HTTP/1.1 %d", &code);
    
    if (code == 0) { //获取不到，可能是1.0 
        sscanf(src, "HTTP/1.0 %d", &code);
    }

    return 0;
}

//获取http协议头的长度，-1：获取失败
int getHttpHeadLength(const char* src) {
    char* searchStr ="\r\n\r\n";
    char* pos = strstr((char*)src, searchStr);

    if(NULL == pos){
        return -1;
    }

    return ((pos-src) + 4);
}

//获取http协议的数据长度字段,-1:can not find；如果是chunked类型的数据，返回1
int getHttpDataLength(const char* src, int srcLen) {

    char* searchStartStr = "Content-Length:";
    char* searchEndStr = "\r\n";
    char* startPos = strstr((char*)src, searchStartStr);

    if(NULL == startPos){ //找不到Content-Length的情况，检查是否是chunked模式
        //Transfer-Encoding: chunked
        char* pos = strstr((char*)src, "Transfer-Encoding: chunked");
        if(NULL == pos){
            wxLogError("getHttpDataLength failed! cannot find \"Content-Length\" or \"Transfer-Encoding\", src[%s]", src);
            return -1;
        }

        char* headEndPos = strstr((char*)src, "\r\n\r\n");
        if(NULL == headEndPos) { //如果找不到头结尾，说明头都还没有接收完成，返回1
            return 1;
        }
        
        int dataLength = getHttpChunkLength(headEndPos + 4);
        dataLength += 2; //加上数据trunk结束符
        //dataLength += 5; //最后结束trunk("0\r\n\r\n")的长度
        dataLength += 5; //加上长度trunk("XXX\r\n")的长度
        return dataLength;
    }

    startPos += strlen(searchStartStr);
    char* endPos = strstr(startPos, searchEndStr);
    if(NULL == endPos){
        return -1;
    }
    int len = endPos - startPos;
    char* buf = new char(len+1);
    memset(buf, 0, sizeof(buf));
    strncpy(buf, startPos, len);
    buf[len]=0;

    return atoi(buf);
}


//获取第一个chunk块的长度；src：解析的数据块，必须已经去掉http头数据
int getHttpChunkLength(const char* dataSrc) {
    char* searchEnd = "\r\n"; //结束标记
    char* endPos = strstr((char*)dataSrc, searchEnd);

    if(NULL == endPos) {//找不到行结束符，说明数据还未接收完，返回1
        return 1;
    }
    
    //如果没有结束符\0，继续搜索？
    int len = endPos - dataSrc;
    char* buf = new char(len+1);
    memset(buf, 0, sizeof(buf));
    strncpy(buf, dataSrc, len);
    buf[len]=0;
    
    int trunk1length = hex_to_decimal(buf, len);//第一个trunk的长度
    return trunk1length;
}

int hex_char_value(char c)   
{   
    if(c >= '0' && c <= '9')   
        return c - '0';   
    else if(c >= 'a' && c <= 'f')   
        return (c - 'a' + 10);   
    else if(c >= 'A' && c <= 'F')   
        return (c - 'A' + 10);   
    assert(0);   
    return 0;   
}   
int hex_to_decimal(const char* szHex, int len) {   
    int result = 0;   
    for(int i = 0; i < len; i++)   
    {   
        result += (int)pow((float)16, (int)len-i-1) * hex_char_value(szHex[i]);   
    }   
    return result;
}  


int CRTData::ConnectSocket(std::string proxyIP, int proxyPort){

    // 加载Win Socket
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(1, 1);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0){
        return err;
    }

    if ( LOBYTE( wsaData.wVersion ) != 1 || HIBYTE( wsaData.wVersion ) != 1 ){
        return -1; 
    }

    // 创建Socket
    m_Socket = socket(AF_INET, SOCK_STREAM, 0);
    if(INVALID_SOCKET == m_Socket){
        return -1;
    }

    //int i = 1024 ;//允接收缓冲区下限为 1k,对select函数没用
    //setsockopt(m_Socket, SOL_SOCKET, SO_RCVLOWAT,  (const char*)&i, sizeof(i)) ;  

    // 连接服务器
    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = inet_addr(proxyIP.c_str());
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(proxyPort);

    if( SOCKET_ERROR == connect(m_Socket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR)) ){
        wxLogError("connect failed! errno[%d]", WSAGetLastError());
        return WSAGetLastError();
    }

    //closesocket(sockClient);
    //WSACleanup();
    return 0;
}

int CRTDataNoProxy::ConnectSepSocket(string& hostName){
    
    char ipAddr[20] = {0};
    if( Domain2Ip(hostName.c_str(), ipAddr) < 0 ){
        return -1;
    }
    m_hostIp = ipAddr;

    return ConnectSocket(m_hostIp, m_hostPort);
}
CRTDataNoProxy::CRTDataNoProxy(int port) : m_hostPort(port) {
}

int CRTDataNoProxy::GetHttpData(string url, string& allGetData, FILE* flstm)
{
    //url = "http://stock.finance.sina.com.cn/hkstock/api/jsonp.php/var%20tableData%20=%20/FinanceStatusService.getBalanceSheetForjs?symbol=00013&balanceSheet=zero";
    string::size_type hostBegin = url.find("//", 0);
    string::size_type hostEnd = url.find("/", hostBegin+2);
    string host = url.substr(hostBegin+2, hostEnd-hostBegin-2);
    string uri = url.substr(hostEnd, -1);

    if( resetSocket(host) < 0 ){
        wxLogError("resetSocket error! host[%s]", host);
        return -1;
    }

    string httpHead="GET "+trim(uri)+" HTTP/1.1\r\n";
    //httpHead += "Host: stock.finance.sina.com.cn\r\n";
    httpHead += "Host: "+host+"\r\n";
    httpHead += "Connection: keep-alive\r\n";
    httpHead += "Cache-Control: max-age=0\r\n";
    httpHead += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n";
    //httpHead += "Upgrade-Insecure-Requests: 1\r\n";
    httpHead += "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.118 Safari/537.36\r\n";
    //httpHead += "Accept-Encoding: gzip, deflate, sdch\r\n";  //要求服务器对内容进行gzip编码之后再传回来，会变乱码
    httpHead += "Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\n\r\n";
    //httpHead += "Cookie: vjuids=-2d3d3dfb2.148abf83364.0.f2ada3db; U_TRS1=00000027.6ae62e00.5423d50b.c8de8931; SINAGLOBAL=58.251.152.7_1423047100.132662; SGUID=1423049473600_7576659; UOR=,,; lxlrttp=1448756959; sso_info=v02m6alo5qztKWRk5ilkKSQpY6UjKWRk5ylkJOYpY6EmKWRk5iljoSEpY6TlKWRk6ClkKOUpY6DkKadlqWkj5OEt4yTiLSMg6C1jLOUwA==; hk_visited_stocks=00984; FIN_ALL_VISITED=00001%2C03828%2C00336%2C00035%2C000880%2C000296%2C00984%2C00900%2C03368%2C00392%2C00603%2C00956%2C00912%2C03669%2C00639%2C08008%2C01247%2C000829%2C003948%2C000639%2C003899%2C001623%2C00825%2C02198%2C01259%2C00608%2C00999%2C02877%2C01623%2C00589%2C00393%2C06822%2C00189%2C01300%2C03336%2C00894%2C00255%2C01899%2C01358%2C00384%2C00196%2C03899%2C00206%2C00358; SUBP=0033WrSXqPxfM725Ws9jqgMF55529P9D9WhvwoQAEY5c7VN6QcugplEm; ALF=1482060132; ULV=1450524136010:15:6:6::1450437546562; vjlast=.1450524140.11; U_TRS2=00000024.17925a2d.56756581.bcbe757a; dpha=usrmdinst_12\r\n";
    //httpHead += "If-Modified-Since: Sat, 19 Dec 2015 14:34:05 GMT\r\n\r\n";

    int sRet=-1;
    sRet = send(m_Socket, httpHead.c_str(), httpHead.length(), 0);
    if(SOCKET_ERROR == sRet) { //发送数据失败后，断开连接，再重新连接
        wxLogError("send failed! errcode[%d], httpHead[%s]", WSAGetLastError(), httpHead.c_str());
        return -1;
    }

    int httpCode;
    if(0 > Recv(m_Socket, allGetData,httpCode, flstm)) {
        wxLogError("Recv error!");
        return -1;
    }

    if (httpCode != 200) {
        //wxLogError("http code is not 200! allGetData[%s]", allGetData);
        wxLogError("http code is not 200! url[%s]", url.c_str());
        return -5;
    }

    return 1;
}

//never use, use dnews process
void CRTData::LoadOneDayNews( wxDateTime &datetime, string outDir, int beginNum, int endNum) {
    string dateStr1 = datetime.Format("%Y/%m%d");
    string dateStr2 = datetime.Format("%Y%m%d");

    string dirStr = outDir+"\\"+dateStr2+"\\";
    if( _mkdir(dirStr.c_str()) != 0 ){
        int errnum = errno;
        if(errnum != EEXIST){
            wxLogError("make dir failed! dir[%s], errno[%d]", dirStr.c_str(),errnum);
            return;

        }else{ //目录已存在，选择最大的文件名，再从此文件名开始load。
            wxDir dirObj(dirStr.c_str());
            if ( !dirObj.IsOpened() ){
                wxLogError("open dir failed! dir[%s], errno[%d]", dirStr.c_str(),errno);

            }else{
                wxArrayString filelist;
                dirObj.GetAllFiles(dirStr, &filelist);
                filelist.Sort(false);
                std::string filename = filelist.Last();
                string maxNumStr = filename.substr(filename.size()-9, 9).substr(0, 3);
                beginNum = atoi(maxNumStr.c_str());
            }
        }
    }

    //http://www.hkexnews.hk/listedco/listconews/sehk/2015/1224/LTN20151224413_C.pdf
    string url= "http://www.hkexnews.hk/listedco/listconews/sehk/"+dateStr1+"/";
    char fileName[256] = {0};
    for(int i=beginNum; i<=endNum; ++i){
        
        memset(fileName, 0, sizeof(fileName));
        sprintf_s(fileName,sizeof(fileName), "LTN%s%03d_C.pdf",dateStr2.c_str(),i);

        FILE* stream = fopen((dirStr + fileName).c_str(), "wb");
        if(stream == NULL){
            wxLogError("open file[%s] failed!", fileName);
            continue;
        }

        string test;
        if(GetHttpData(url+fileName,test,stream) < 0){
            fclose(stream);
            remove((dirStr + fileName).c_str());
            continue;
        }
        fclose(stream);

    }

    wxLogInfo("LoadOneDayNews finish!");
}