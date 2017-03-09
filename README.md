# cubiconews

#### This module is still under heavy Development

CubicoNews is a newsletter module for Drupal 7 that helps plain users to create and send newsletter without messing with templates and HTML code.


## Features
* Every Newsletter is a node with articles.
* The user just have to select which nodes will be added at the latest active Newsletter by clicking to the "Add to Newsletter" tab inside the node
* Testing Newsletters can be send to a list of predefined email accounts
* "Send now" procedure using Drupal queues
* "Send later" schedule procedure using Drupal queues and cron (partially ready)
* Email accounts manipulation, with multiple selection, blocking, unblocking and removal
* Import / Remove multiple email accounts, from generic text without specific structure needed
* Subscribe block for the page with email field, which first validates the inserted value and then stores it.
* Unsubscribe page
* Headless newsletter page
* Newsletter template can be a HTML/PHP file
* Own mail system setting to be used with [mailsystem](https://www.drupal.org/project/mailsystem) module
* Tracking pixel addition for statistical reasons
* Use specific content types and image fields as article sources
* Multiple permissions for various procedures


## Start using Cubiconews
### Configuration
As soon as you install Cubiconews, you have to configure it, by visiting the **Configuration > CubicoNews Administration**, where you can set which content types can be used, which image fields will be used as main image for each article and the testing email accounts.

When you finish Cubiconews configuration, you are ready to create your newsletters.

### Create a Newsletter and add Articles
To create a new newsletter you just have to add an article to it. If there is no newsletter or none is active, a new newsletter is created automatically and the article is added to it. To add an article just visit the node and use the tab beside '**_Edit_**', with the label '**_Add to Newsletter_**'. Automatically you will be redirected to newsletter's node page, where you will see it's preview.

If you edit this node, you can change the automatic generated title, and edit the articles that are included. You can also add more articles, by specifying their content, such as the photo, the body text and the link where this will lead. The link can be whatever you want. Finally, you can change their order by drag 'n drop.

### Newsletter manipulation
You can also find all the newsletters at the admin page **Configuration > CubicoNews Administration > Newsletters**. By that screen you can see the status of each newsletter, when they were sent and which one is active. If you click on the title you can view the newsletter. At the "operations" column you can select to send the newsletter. By the form you will see, you can "**_Send test_**" emails to the testing accounts. If the newsletter is ready and finalized, you can send it to all the active recipients by the "**_Send now_**", or you can schedule the sending of it for later time.

### Recipients manipulation
You can manipulate the recipients by the admin page **Configuration > CubicoNews Administration > Recipients**. At this page you can see how many active and inactive recipients your list has. After that you will find a list of all the emails. There you will find for each email account it's status, when it registered or removed. You can select multiple accounts by the checkbox at th left, and perform various actions on them, like enable, disable and remove.

### Import recipient email accounts
If you have many emails in a text you can import them by the admin page **Configuration > CubicoNews Administration > Import emails**. This text doesn't have to have a specific structure, but it would be best to have the emails separated by a comma or colon. The import procedure will find the email accounts and will add them in the database. If an email exists, it will not change, unless the "**_Enable existing emails_**" checkbox is checked, which changes their status to enabled.

### Remove emails
If you want to remove multiple emails at once, you can go to the admin page **Configuration > CubicoNews Administration > Remove emails**. You can add a text of emails at the textarea, which doesn't have to have a specific structure, but it would be best to have the emails separated by a comma or colon. The remove procedure will find the email accounts and will remove them from the database. **THIS CAN NOT BE UNDONE**. That's why there is a checkbox, that you have to check if you are sure about the removal.

### Mail System Configuration
I am guessing that you would like your newsletter to include HTML code. The Cubiconews has it's own mail system configuration, which you have to setup, so that your emails will be visible as HTML emails and look as they should. So, you have to go at the ** Configuration > Mail System ** page and add a "**_New Setting_**" by choosing the "**_InterCube media >> Cubiconews module_**" at the "**_Module_**" select box, and fill the "**_Key_**" field with "**_cubiconews_mailkey_**".

When you save this, a new select box will appear at the "**_Mail System Settings_**" with the label "**_Cubiconews module (cubiconews_mailkey key) class_**". Select there the value '**_CubicoNewsMailSystem_**' and click "Save settings" once more, and you are done.

### Template development
Currently the template is a PHP file under the module folder, and the directory "**_themes_**", named "**_newsletter.php_**". There you can have your HTML code and PHP code. An array of all the articles is available, and a few info about the newsletter itself, such as the title. You can use any PHP command or loops to place the articles in the equivalent position.

### Tracking pixel
The tracking pixel is added to the newsletter automatically, when it is sent to each recipient. It includes a user hash based on the newsletter ID and the recipient's email, which makes it unique. When the recipient opens the newsletter, this image of 1x1 pixel is shown and updates the database, where marks the user that viewed it.

To make this work, do not forget to add `<!--tracking_pixel-->` just before the `</body>` tag.


## TODO list
* Create graphic stats where the user can see the evolution of recipients' registrations/removals
* Add Statistics tab at the newsletters' page
* Disable a newsletter after it is sent
* Add specific link on every link in newsletter, that will count the clicks, for statistics
* Setup scheduled email send procedure using cron
* Add fields in the config page for the default messages, like the "Successful registration" or "email removal"
* Create an email that would be send to the user to enable/approve his registration. This should have a token that would expire after specific time, and would help for the validation of the email account
* Email validation should get it's own function
* Add email Groups
