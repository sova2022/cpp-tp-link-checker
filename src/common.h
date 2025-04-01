#pragma once

#include <map>

namespace common {

	static constexpr const char* PROGRAM_NAME = "TP-Link Checker";
	static constexpr const char* VERSION = "1.0.0";

	static constexpr const char* CTRL_C_DETECTED_INFO = "Ctrl+C detected. Exiting...";
	static constexpr const char* PASSWORD = "admin";
    
    struct Key {
        static constexpr const char* LOGIN = "login";
        static constexpr const char* LOGIN_ALT = "login_alt";
    };

    const QString ALT_URL_FORMAT = "https://%1/webpages/login.html";
    const QString DEFAULT_URL_FORMAT = "https://%1";

    const QString JS_CODE_LOGIN(R"(
    (function() {
        let passwordField = document.getElementById("pcPassword");
        let loginButton = document.getElementById("loginBtn");
        if (passwordField && loginButton) {
            passwordField.value = "%1";
            loginButton.click();
            return true;
        } else {
            return false;
        }
    })();
    )");

    const QString JS_CODE_LOGIN_ALT(R"(
    (function() {
        let passwordFields = document.getElementsByClassName("text-text password-text");
        if (passwordFields.length > 5) {
            let passwordField = passwordFields[4];
            passwordField.focus();
            let password = "%1";
            for (let char of password) {
                let event = new InputEvent("input", { bubbles: true });
                passwordField.value += char;
                passwordField.dispatchEvent(event);
            }
            let changeEvent = new Event("change", { bubbles: true });
            let blurEvent = new Event("blur", { bubbles: true });
            passwordField.dispatchEvent(changeEvent);
            passwordField.dispatchEvent(blurEvent);
            document.getElementById("login-btn").focus();
            document.getElementById("login-btn").click();
            return true;
        } else {
            return false;
        }
    })();
    )");

    const std::map<const char*, const QString> LOGIN_VARS{
        { Key::LOGIN, JS_CODE_LOGIN },
        { Key::LOGIN_ALT, JS_CODE_LOGIN_ALT }
    };
}