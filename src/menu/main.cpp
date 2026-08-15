#include "launchermenu.h"
#include "layershell.h"
#include "palette.h"

#include "config.h"

#include <QApplication>

// helm-menu: the Start menu. A layer-shell Overlay popup anchored bottom-left,
// just above the panel. Phase 1 first cut: one instance per invocation; it quits
// after launching an app or on Esc.
int main(int argc, char **argv) {
    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "layer-shell");

    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("helm-menu"));
    app.setDesktopFileName(QStringLiteral("helm-menu"));
    helm::applyAppearance();

    const helm::Config cfg;

    helm::LauncherMenu menu;
    menu.winId(); // realise the platform window

    helm::applyLayerShell(
        menu.windowHandle(), LayerShellQt::Window::LayerOverlay,
        helm::edges(/*top*/ false, /*bottom*/ true, /*left*/ true, /*right*/ false),
        /*exclusiveZone*/ 0, LayerShellQt::Window::KeyboardInteractivityOnDemand,
        QMargins(0, 0, 0, cfg.panelHeight()));

    // Dismiss the Start menu when the app loses focus (user clicked another
    // window or the desktop). A layer-shell Overlay doesn't get widget-level
    // WindowDeactivate, so track application state instead: once we've been
    // active, quit as soon as we go inactive (unless a child popup — the
    // right-click actions menu — is what took focus).
    QObject::connect(&app, &QApplication::applicationStateChanged, &app,
                     [&app](Qt::ApplicationState state) {
                         static bool wasActive = false;
                         if (state == Qt::ApplicationActive) {
                             wasActive = true;
                         } else if (wasActive && !QApplication::activePopupWidget()) {
                             app.quit();
                         }
                     });

    menu.show();
    return app.exec();
}
