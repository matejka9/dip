<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Diplomová práca úvod</title>
    <link href="assets/styly.css" rel="stylesheet" media='screen'>
  </head>

  <body>

    <header>
      <h1>Úvod</h1>
    </header>

    <?php include 'components/nav.php';?>

    <section>
      <p><h1>Vitajte na stránke pre moju diplomovú prácu.</h1></p>

      <p>
        <h2>Téma</h2>
        Lokalizačný systém pre mobilného robota<br>
        Localization System for Mobile Robot
      </p>

      <p>
        <h2>Popis</h2>
        Základná úloha pre mobilného robota pohybujúceho sa v známom hoci dynamickom prostredí s cieľom plnenia zadanej úlohy je čo najpresnejšia lokalizácia. Vzhľadom na povahu dát zo senzorov je výhodné využiť pravdepodobnostné reprezentácie a algoritmy. V akademickom roku 2018/2019 sa robotická skupina na Katedre aplikovanej informatiky zúčastní robotickej súťaže SICK Robot Day. Cieľom práce je vyskúmať, navrhnúť a implementovať vhodný lokalizačný algoritmus využívajúci dáta z hĺbkového laserového senzora a navigáciu podľa naplánovanej stratégie pre mobilného robota do tejto súťaže. Výsledkom bude univerzálny lokalizačný systém, ktorý je použiteľný aj v iných podobných scenároch. Predpokladáme využitie výkonnej výpočtovej platformy GPU NVIDIA Jetson TX2.
      </p>

      <p>
        <h2>Zdroje</h2>
        <a href="documents/mote_carlo_localization.pdf" target="_blank">Robust Monte Carlo localization for mobile robots</a> <br>
        <a href="documents/probablistic_map_review.pdf" target="_blank">Mobile Robot Localization: A Review of Probabilistic Map- Based Techniques</a> <br>
        <a href="documents/map_based_navigation.pdf" target="_blank">Map-based navigation in mobile robots</a> <br>
        <a href="https://watermark.silverchair.com/gkx1313.pdf?token=AQECAHi208BE49Ooan9kkhW_Ercy7Dm3ZL_9Cf3qfKAc485ysgAAAZ4wggGaBgkqhkiG9w0BBwagggGLMIIBhwIBADCCAYAGCSqGSIb3DQEHATAeBglghkgBZQMEAS4wEQQMGa04uL3lz4YnroDXAgEQgIIBUcB1pmQiXmWYedIJqNDzLX5EzD75SQvFTIgVoJ9sgox9Cl3c1iBZHcA55Cp51FomBUu0icVsO1fQMSozLqowOZpkm-jkmGf93HZktLh4_gDLvpWiX9M1jpyGO7BXx33hGzsIGPnTK6TyN8yMe-OZZOw5ov6kqzqfP_piP0Hfp7hRbR0jpjmfUdTfv_NZ628qjUE0y-3RDDv8dfE4pskkMJ-cF9H-QfGYxHO2WCFTjhEMgM8U4C6DnwvD7sN6VX7rPG68hPwpJwmMgPnIYTY8KSRVQmALv2D4oc87_IrLMsuQKGJvttv9WCv_Eaw5s39t1L-h4CZBV0CHVxXbcObvXpzPorD6k45TkOsPOD3bocVP4ZJE6yjs2Pl93JDNE9Z7PsF8xGrh2pj2g-ckVNExwbkoIRv3cY7yH3rqvQL-vt3UctELY098XAN9N5c_jB2Xgmo" target="_blank">HipMCL: a high-performance parallel implementation of the Markov clustering algorithm for large-scale networks</a> <br>
        PROBABILISTIC ROBOTICS (kniha)<br>
        The CUDA HANDBOOK. A Comprehensive Guide to GPU Programming (kniha)
      </p>

      <p>
        <h2>Školitel</h2>
        Mgr. Pavel Petrovič, PhD.
      </p>
    </section>

    <?php include 'components/aside.php';?>
    <?php include 'components/footer.php';?>

  </body>
</html>
