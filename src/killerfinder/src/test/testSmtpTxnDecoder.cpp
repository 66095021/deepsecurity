
#include <iostream>
#include <string.h>
#include "vmime/vmime.hpp"
#include "vmime/platforms/posix/posixHandler.hpp"

using namespace std;
const string mail= "From: <zzk_hw@sina.com>\r\n"
                   "To: <59746423@qq.com>\r\n"
                   "Subject:this is a test\r\n"
                   "MIME-Version: 1.0\r\n"
                   "Content-Type: multipart/alternative;boundary=\"----=_NextPart_000_0003_01C7118D.38E01920\"\r\n"
                   "\r\n"
                   "This is a multi-part message in MIME format.\r\n"
                   "\r\n"
                   "------=_NextPart_000_0003_01C7118D.38E01920\r\n"
                   "Content-Type: text/plain;charset=\"gb2312\"\r\n"
                   "Content-Transfer-Encoding:base64\r\n"
                   "\r\n"
                   "MTIzMTIzMTIz\r\n"
                   "\r\n"
                   "------=_NextPart_000_0003_01C7118D.38E01920\r\n"
                   "Content-Type: text/html;charset=\"gb2312\"\r\n"
                   "Content-Transfer-Encoding:base64\r\n"
                   "\r\n"
                   "PCFET0NUWVBFIEhUTUwgUFVCTElDICItLy9XM0MvL0RURCBIVE1MIDQuMCBUcmFuc2l0aW9uYWwv"
                   "L0VOIj4NCjxIVE1MPjxIRUFEPg0KPE1FVEEgaHR0cC1lcXVpdj1Db250ZW50LVR5cGUgY29udGVu"
                   "dD0idGV4dC9odG1sOyBjaGFyc2V0PWdiMjMxMiI+DQo8TUVUQSBjb250ZW50PSJNU0hUTUwgNi4w"
                   "MC4yOTAwLjI5OTUiIG5hbWU9R0VORVJBVE9SPg0KPFNUWUxFPjwvU1RZTEU+DQo8L0hFQUQ+DQo8"
                   "Qk9EWSBiZ0NvbG9yPSNmZmZmZmY+DQo8RElWPjxGT05UIHNpemU9Mj4xMjMxMjMxMjM8L0ZPTlQ+"
                   "PC9ESVY+PC9CT0RZPjwvSFRNTD4NCg==\r\n"
                   "\r\n"
                   "------=_NextPart_000_0003_01C7118D.38E01920--\r\n"
                   /*".\r\n"*/;


const string mail2="HELO domain.com\r\n"
                   "MAIL FROM:<zzk_hw@sina.com>\r\n"
                   "RCPT TO:<59746423@qq.com>\r\n"
                   "DATA\r\n"
                   "From: <zzk_hw@sina.com>\r\n"
                   "To: <59746423@qq.com>\r\n"
                   "Subject:test1\r\n"
                   "MIME-Version: 1.0\r\n"
                   "Content-Type: multipart/alternative;boundary=\"----=_NextPart_000_0003_01C7118D.38E01920\"\r\n"
                   "\r\n"
                   "This is a multi-part message in MIME format.\r\n"
                   "\r\n"
                   "------=_NextPart_000_0003_01C7118D.38E01920\r\n"
                   "Content-Type: text/plain;charset=\"gb2312\"\r\n"
                   "Content-Transfer-Encoding:base64\r\n"
                   "\r\n"
                   "MTIzMTIzMTIz\r\n"
                   "\r\n"
                   "------=_NextPart_000_0003_01C7118D.38E01920\r\n"
                   "Content-Type: text/html;charset=\"gb2312\"\r\n"
                   "Content-Transfer-Encoding:base64\r\n"
                   "\r\n"
                   "PCFET0NUWVBFIEhUTUwgUFVCTElDICItLy9XM0MvL0RURCBIVE1MIDQuMCBUcmFuc2l0aW9uYWwv"
                   "L0VOIj4NCjxIVE1MPjxIRUFEPg0KPE1FVEEgaHR0cC1lcXVpdj1Db250ZW50LVR5cGUgY29udGVu"
                   "dD0idGV4dC9odG1sOyBjaGFyc2V0PWdiMjMxMiI+DQo8TUVUQSBjb250ZW50PSJNU0hUTUwgNi4w"
                   "MC4yOTAwLjI5OTUiIG5hbWU9R0VORVJBVE9SPg0KPFNUWUxFPjwvU1RZTEU+DQo8L0hFQUQ+DQo8"
                   "Qk9EWSBiZ0NvbG9yPSNmZmZmZmY+DQo8RElWPjxGT05UIHNpemU9Mj4xMjMxMjMxMjM8L0ZPTlQ+"
                   "PC9ESVY+PC9CT0RZPjwvSFRNTD4NCg==\r\n"
                   "\r\n"
                   "------=_NextPart_000_0003_01C7118D.38E01920--\r\n"
                   ".\r\n"
                   
                   "SEND FROM:<zzk_hw@sina.com>\r\n"
                   "RCPT TO:<59746423@qq.com>\r\n"
                   "DATA\r\n"
                   "From: <zzk_hw@sina.com>\r\n"
                   "To: <59746423@qq.com>\r\n"
                   "Subject:test2\r\n"
                   "\r\n"
                   "mail body 2\r\n"
                   ".\r\n"
                   
                   "SOML FROM:<zzk_hw@sina.com>\r\n"
                   "RCPT TO:<59746423@qq.com>\r\n"
                   "DATA\r\n"
                   "From: <zzk_hw@sina.com>\r\n"
                   "To: <59746423@qq.com>\r\n"
                   "Subject:test3\r\n"
                   "\r\n"
                   "mail body 3\r\n"
                   ".\r\n"
                   
                   "SAML FROM:<zzk_hw@sina.com>\r\n"
                   "RCPT TO:<59746423@qq.com>\r\n"
                   "DATA\r\n"
                   "From: <zzk_hw@sina.com>\r\n"
                   "To: <59746423@qq.com>\r\n"
                   "Subject:test4\r\n"
                   "\r\n"
                   "mail body 4\r\n"
                   ".\r\n";

string mail3 = "";

vector<string> bodys;

void makeMailString()
{
    std::cout << std::endl;

    // VMime initialization
    vmime::platform::setHandler<vmime::platforms::posix::posixHandler>();

    try
    {
        vmime::messageBuilder mb;

        // Fill in the basic fields
        mb.setExpeditor(vmime::mailbox("Expeditor@somewhere.com"));

        vmime::addressList to;
        to.appendAddress(vmime::create <vmime::mailbox>("Recipients@elsewhere.com"));
        to.appendAddress(vmime::create <vmime::mailbox>("someone@elsewhere.com"));
        mb.setRecipients(to);

        vmime::addressList bcc;
        bcc.appendAddress(vmime::create <vmime::mailbox>("you-bcc@nowhere.com"));
        bcc.appendAddress(vmime::create <vmime::mailbox>("bcc@nowhere.com"));
        mb.setBlindCopyRecipients(bcc);

        mb.setSubject(vmime::text("My first message generated with vmime::messageBuilder"));

        // Message body
        mb.getTextPart()->setText(vmime::create <vmime::stringContentHandler>(
            "I'm writing this short text to test message construction " \
            "with attachment, using the vmime::messageBuilder component."));

        // Adding an attachment
        /**/
        vmime::ref <vmime::fileAttachment> a1 = vmime::create <vmime::fileAttachment>
        (
            "/root/testString",                               // full path to file
            vmime::mediaType("application/octet-stream"),   // content type
            vmime::text("bin attachment")              // description
        );

        a1->getFileInfo().setFilename("testString");
        a1->getFileInfo().setCreationDate(vmime::datetime("30 Apr 2003 14:30:00 +0200"));

        mb.attach(a1);

         vmime::ref <vmime::fileAttachment> a2 = vmime::create <vmime::fileAttachment>
        (
            "/root/test.txt",                               // full path to file
            vmime::mediaType("text/plain"),   // content type
            vmime::text("txt attachment")              // description
        );

        a2->getFileInfo().setFilename("test.txt");
        a2->getFileInfo().setCreationDate(vmime::datetime("30 Apr 2003 14:30:00 +0200"));

        mb.attach(a2);
        /**/

        // Construction
        vmime::ref <vmime::message> msg = mb.construct();

        // Raw text generation
       
        vmime::string dataToSend = msg->generate();

        std::cout << "Generated message:" << std::endl;
        std::cout << "==================" << std::endl;
        std::cout << std::endl;
        std::cout << dataToSend << std::endl;

         mail3 = msg->generate();     
    }
    // VMime exception
    catch (vmime::exception& e)
    {
        std::cout << "vmime::exception: " << e.what() << std::endl;
        throw;
    }
    // Standard exception
    catch (std::exception& e)
    {
        std::cout << "std::exception: " << e.what() << std::endl;
        throw;
    }

    std::cout << std::endl;

}



void getMailDatas(const string& buf){
   const char* startTag = "\r\nDATA\r\n";
   const char* endTag   = "\r\n.\r\n";
   size_t startTagLen = strlen(startTag);
   size_t endTagLen   = strlen(endTag);
   
   const char* begin = buf.c_str();
   size_t len = buf.size();
   size_t off1 = 0;
   size_t off2 = 0;
   
   while (off1 < len-startTagLen+1){
       if (0 == strncasecmp(begin+off1, startTag, startTagLen)){
           cout<<"find a DATA "<<off1<<endl;
           off2 = off1; 
           while (off2 < len-endTagLen+1){
               if (0 == strncasecmp(begin+off2, endTag, endTagLen)){
                   cout<<"find a . "<<off2<<endl;
                   string s(begin+off1+startTagLen, off2-off1-startTagLen);
                   bodys.push_back(s);
                   //cout<<s<<endl;
                   break;
               }
               off2++;
           }
           off1 = off2;
       }else{
           off1++;
       }
   }
}


void test1(const string& str)
{

    std::cout <<"test1"<<std::endl;

    // VMime initialization
    vmime::platform::setHandler<vmime::platforms::posix::posixHandler>();

    try
    {
        

        vmime::utility::ref <vmime::message> msg = vmime::create <vmime::message>();
        msg->parse(str);
        
        vmime::utility::ref <vmime::header> h = msg->getHeader();
        const vector <vmime::utility::ref<vmime::headerField>> hl = h->getFieldList();
        for (size_t i=0; i<hl.size();i++){
            cout<<"name :"<<hl[i]->getName()<<endl;
            cout<<"value:"<<hl[i]->getValue()->generate()<<endl;
        }


        vmime::messageParser mp(str);
        cout<<"date "<<mp.getDate().generate()<<endl;
        cout<<"date "<<mp.getExpeditor().generate()<<endl;
        cout<<"date "<<mp.getBlindCopyRecipients().generate()<<endl;
        cout<<"date "<<mp.getRecipients().generate()<<endl;
        cout<<"date "<<mp.getSubject().generate()<<endl;
        cout<<"date "<<mp.getCopyRecipients().generate()<<endl;


  /*      vmime::addressList adds = mp.getRecipients();
        for (int i=0; i < adds.getAddressCount(); i++){
            vmime::utility::ref<vmime::address> add = adds.getAddressAt(i);
            cout<<"addr "<<i<<" "<<add<<endl;
        }
        

        //vmime::utility::ref <vmime::mailboxList> mails1 = mp.getRecipients().toMailboxList();
        const vmime::mailboxList& mails = *(mp.getRecipients().toMailboxList());
        cout<<"count "<<mails.getMailboxCount()<<endl;
        for (int i=0; i<mails.getMailboxCount(); i++){
            string result;
            vmime::utility::outputStreamStringAdapter ossa(result);
            const vmime::mailbox& m = *(mails.getMailboxAt(i));
            m.generate(ossa);
            cout<<"mail "<<i<<" "<<result<<endl;
        }
*/        

        for (int i = 0 ; i < mp.getAttachmentCount() ; ++i)
        {
           const vmime::attachment& att = *mp.getAttachmentAt(i);

           // Media type (content type) is in "att.getType()"
           cout<<"type:"<<att.getType().getType()<<" subtype:"<<att.getType().getSubType()<<endl;
           // Name is in "att.getName()"
           cout<<"name:"<<att.getName().generate()<<endl;
           cout<<"name:"<<att.getName().getBuffer()<<endl;
           // Description is in "att.getDescription()"
           cout<<"descreption:"<<att.getDescription().generate()<<endl;
           cout<<"descreption:"<<att.getDescription().getWholeBuffer()<<endl;
           // Data is in "att.getData()"
           string result;
           vmime::utility::outputStreamStringAdapter ossa(result);
           att.getData()->extract(ossa);
           cout<<result<<endl;
        }


        

        // Enumerate text parts
        for (int i = 0 ; i < mp.getTextPartCount() ; ++i)
        {
            const vmime::textPart& part = *mp.getTextPartAt(i);

            // Output content-type of the part
            std::cout << part.getType().generate() << std::endl;

            // text/html
            if (part.getType().getSubType() == vmime::mediaTypes::TEXT_HTML)
            {
                const vmime::htmlTextPart& hp = dynamic_cast<const vmime::htmlTextPart&>(part);

                // HTML text is in "hp.getText()"
                string result1;
                vmime::utility::outputStreamStringAdapter ossa1(result1);
                hp.getText()->extract(ossa1);
                cout<<endl<<"["<<result1<<"]"<<endl;

                string result2;
                vmime::utility::outputStreamStringAdapter ossa2(result2);
                hp.getPlainText()->extract(ossa2);
                cout<<endl<<"["<<result2<<"]"<<endl;
                
                // Corresponding plain text is in "hp.getPlainText()"

                // Enumerate embedded objects (eg. images)
                for (int j = 0 ; j < hp.getObjectCount() ; ++j)
                {
                    const vmime::htmlTextPart::embeddedObject& obj = *hp.getObjectAt(j);

                    // Identifier (content-id or content-location) is in "obj.getId()"
                    // Object data is in "obj.getData()"
                    string result;
                    vmime::utility::outputStreamStringAdapter ossa(result);
                    obj.getData()->extract(ossa);
                    cout<<endl<<result<<endl;
                }
            }
            // text/plain
            else if (part.getType().getSubType()==vmime::mediaTypes::TEXT_PLAIN)
            {
                const vmime::textPart& tp = dynamic_cast<const vmime::textPart&>(part);

                // Text is in "tp.getText()"
                string result3;
                vmime::utility::outputStreamStringAdapter ossa3(result3);
                tp.getText()->extract(ossa3);
                cout<<endl<<result3<<endl;
            }
        }
    }
    // VMime exception
    catch (vmime::exception& e)
    {
        std::cout << "vmime::exception: " << e.what() << std::endl;
        throw;
    }
    // Standard exception
    catch (std::exception& e)
    {
        std::cout << "std::exception: " << e.what() << std::endl;
        throw;
    }

    std::cout << std::endl;

    


}




int main(int argc, char** argv){
    makeMailString();
    test1(mail3);/*
    getMailDatas(mail2);
    for(size_t i = 0; i < bodys.size(); i++){
        test1(bodys[i]);
        test2(bodys[i]);
    }*/

}
