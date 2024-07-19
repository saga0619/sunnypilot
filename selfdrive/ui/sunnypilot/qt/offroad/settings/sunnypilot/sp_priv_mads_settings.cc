#include "selfdrive/ui/sunnypilot/qt/offroad/settings/sunnypilot/sp_priv_mads_settings.h"

#include <tuple>
#include <vector>

MadsSettings::MadsSettings(QWidget* parent) : QWidget(parent) {
  QVBoxLayout* main_layout = new QVBoxLayout(this);
  main_layout->setContentsMargins(50, 20, 50, 20);
  main_layout->setSpacing(20);

  // Back button
  PanelBackButton* back = new PanelBackButton();
  connect(back, &QPushButton::clicked, [=]() { emit backPress(); });
  main_layout->addWidget(back, 0, Qt::AlignLeft);

  ListWidgetSP *list = new ListWidgetSP(this, false);
  // param, title, desc, icon
  std::vector<std::tuple<QString, QString, QString, QString>> toggle_defs{
    {
      "AccMadsCombo",
      tr("Enable ACC+MADS with RES+/SET-"),
      QString("%1<br>"
              "<h4>%2</h4>")
      .arg(tr("Engage both M.A.D.S. and ACC with a single press of RES+ or SET- button."))
      .arg(tr("Note: Once M.A.D.S. is engaged via this mode, it will remain engaged until it is manually disabled via the M.A.D.S. button or car shut off.")),
      "../assets/offroad/icon_blank.png",
    },
    {
      "MadsCruiseMain",
      tr("Toggle M.A.D.S. with Cruise Main"),
      tr("Allows M.A.D.S. engagement/disengagement with \"Cruise Main\" cruise control button from the steering wheel."),
      "../assets/offroad/icon_blank.png",
    }
  };

  // Disengage Lateral on Brake (DLOB)
  std::vector<QString> dlob_settings_texts{tr("Remain Active"), tr("Pause Steering")};
  dlob_settings = new ButtonParamControlSP(
    "DisengageLateralOnBrake",
    tr("Steering Mode After Braking"),
    tr("Choose how Automatic Lane Centering (ALC) behaves after the brake pedal is manually pressed in sunnypilot.\n\n"
       "Remain Active: ALC will remain active even after the brake pedal is pressed.\nPause Steering: ALC will be paused after the brake pedal is manually pressed."),
    "",
    dlob_settings_texts,
    500);
  dlob_settings->showDescription();
  list->addItem(dlob_settings);

  for (auto &[param, title, desc, icon] : toggle_defs) {
    auto toggle = new ParamControlSP(param, title, desc, icon, this);

    list->addItem(toggle);
    toggles[param.toStdString()] = toggle;

    // trigger offroadTransition when going onroad/offroad
    connect(uiStateSP(), &UIStateSP::offroadTransition, toggle, &ParamControlSP::setEnabled);
  }

  // trigger offroadTransition when going onroad/offroad
  connect(uiStateSP(), &UIStateSP::offroadTransition, dlob_settings, &ButtonParamControlSP::setEnabled);

  main_layout->addWidget(new ScrollViewSP(list, this));
}

void MadsSettings::showEvent(QShowEvent *event) {
  updateToggles();
}

void MadsSettings::updateToggles() {
  if (!isVisible()) {
    return;
  }

  const bool is_offroad = !uiStateSP()->scene.started;
  const bool enable_mads = params.getBool("EnableMads");
  const bool enabled = is_offroad && enable_mads;

  toggles["AccMadsCombo"]->setEnabled(enabled);
  toggles["MadsCruiseMain"]->setEnabled(enabled);
  dlob_settings->setEnabled(enabled);
}
