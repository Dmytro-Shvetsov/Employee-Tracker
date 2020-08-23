import React from 'react';
import axios from 'axios';
import * as authService from '../../../services/authService';
import ImageGallery from 'react-image-gallery';
import {Chart} from '../../common/index'
import {NotFound} from '../../Pages/index';
import {Spinner} from 'reactstrap';
import 'react-image-gallery/styles/css/image-gallery.css';


export default class UserActivityLogs extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            status: "offline",
            data: {
                employee_id: props.match.params.id,
                screenshotSince: (new Date(Date.now() - 86400000)).toISOString(), // 1 day ago
                websitesSince: (new Date(Date.now() - 3600000)).toISOString(), // 1 hour ago
            },
            allDataLoaded: false,
        };
        this._isMounted = false;
        this.reqSource = undefined;

        this.screenshotHeadingRef = React.createRef();
        this.domainsHeadingRef = React.createRef();
    }

    setState = (...args) => {
        if (this._isMounted) {
            super.setState(...args);
        }
    };

    cancelPreviousRequests = async () => {
        if (this.reqSource) {
            this.reqSource.cancel();
        }
        this.reqSource = axios.CancelToken.source();
    };

    handlePageAnchorClick = (event, ref) => {
        event.preventDefault();
        window.scrollTo(0, ref.current.offsetTop);
    };

    loadProfileInfo = async () => {
        await this.cancelPreviousRequests();
        const {data} = this.state;
        try {
            const response = await authService.getUserProfile({user_id: data.employee_id}, this.reqSource.token);
            const profile = JSON.parse(response.data);
            console.log(profile);
            this.setState({profile});
        } catch (error) {
            console.log(error.message);
            if (error.response.data !== undefined) {
                console.log(error.response.data);
            }
            this.setState({error:error.response.data.detail});
        }
    };

    loadScreenshotLogs = async () => {
        await this.cancelPreviousRequests();
        const {data} = this.state;
        try {
            const response = await authService.getUserScreenshotActivityLogs(
                {...data, since: data.screenshotSince},
                this.reqSource.token
            );
            const screenshots = JSON.parse(response.data);
            console.log(screenshots);
            this.setState({screenshots});
        } catch (error) {
            console.log(error.message);
            if (error.response.data !== undefined) {
                console.log(error.response.data);
                this.setState({error:error.response.data.detail});
            }
        }
    };

    loadDomainLogs = async () => {
        await this.cancelPreviousRequests();
        const {data} = this.state;
        try {
            const response = await authService.getUserDomainActivityLogs(
                {...data, since: data.websitesSince},
                this.reqSource.token
            );
            const hostnameCount = JSON.parse(response.data);
            console.log(hostnameCount);
            this.setState({hostnameCount});
        } catch (error) {
            console.log(error.message);
            if (error.response.data !== undefined) {
                console.log(error.response.data);
                this.setState({error:error.response.data.detail});
            }
        }
    };

    initWebsocket() {
        const { host, protocol } = location;
        const url = `${protocol === "http:" ? "ws" : "wss"}://${host}/master/`;
        console.log("Connecting to ", url);

        const socket = new WebSocket(url);

        socket.onopen = event => {
            socket.send(JSON.stringify({
                type: "employee.ping",
                user_id: this.state.data.employee_id
            }));
        };

        socket.onmessage = event => {
            const data = JSON.parse(event.data);
            console.log(data);
            switch(data.type) {
                case "websocket.accept": {
                    console.log("Websocket connection established. ", data);
                    break;
                }
                case "employee.status": {
                    this.setState({status: data.status});
                    break;
                }
                default: {
                    console.log("Unknown message of type: ", data.type);
                }
            }
        };
        socket.onclose = event => {
            console.log("Websocket connection closed. ", event.reason, event);
        };

        socket.onerror = event => {
            console.log("Websocket error. ", event.reason, event);
        };

        this.setState({socket});
    }

    async componentDidMount() {
        this._isMounted = true;

        // set up profile related stuff
        await this.loadProfileInfo();
        this.initWebsocket();

        await this.loadScreenshotLogs();
        await this.loadDomainLogs();

        this.setState({allDataLoaded: true});
    }

    async componentWillUnmount() {
        this._isMounted = false;
        await this.cancelPreviousRequests();
    }

    renderProfile() {
        const {profile, status} = this.state;
        return (
            <React.Fragment>
                <div className="col-12 col-lg-8 col-md-6">
                    <div>
                        <h1 className="mb-0 text-truncated">
                            {profile.full_name}
                        </h1>
                    </div>
                    <p className="lead">{profile.work_place}</p>
                    <div className="card-text">
                        <h6>
                            Contents
                        </h6>
                        <ul>
                            <li>
                                <a
                                    href="#"
                                    onClick={e => this.handlePageAnchorClick(e, this.screenshotHeadingRef)}
                                >
                                    Screenshots
                                </a>
                            </li>
                            <li>
                                <a
                                    href="#"
                                    onClick={e => this.handlePageAnchorClick(e, this.domainsHeadingRef)}
                                >
                                    Domains
                                </a>
                            </li>
                        </ul>
                    </div>
                </div>
                <div className="col-12 col-lg-4 col-md-6 text-center">
                    <img src={profile.image} alt="Profile image" id="profile-image-rounded"/>
                    <p>
                        Currently <span className={status}>{status}</span>
                    </p>
                </div>
                <div>

                </div>
            </React.Fragment>
        );
    }

    renderScreenshots() {
        const {screenshots = []} = this.state;

        return (
            <React.Fragment>
                {screenshots.length === 0 ? (
                    <span className="text-muted">
                        There are no screenshots for past 24 hours.
                    </span>
                ) : (
                    <ImageGallery
                        showPlayButton={false}
                        lazyLoad={true}
                        onClick={() => {
                            document.getElementsByClassName("image-gallery-fullscreen-button")[0].click()
                        }}
                        items={screenshots.map(item => ({
                            original: item.image,
                            thumbnail: item.image,
                            description: (new Date(item.date)).toLocaleString()
                        }))
                        }/>
                )}
            </React.Fragment>
        );

    }

    renderDomains = () => {
        const {hostnameCount=[]} = this.state;

        if (hostnameCount.length === 0) {
            return <span className="text-muted">The user haven't visited any websites for the past hours.</span>
        }

        const backgroundColors = [
            "rgba(255, 99, 132, 0.2)",
            "rgba(54, 162, 235, 0.2)",
            "rgba(255, 206, 86, 0.2)",
            "rgba(75, 192, 192, 0.2)",
            "rgba(153, 102, 255, 0.2)",
            "rgba(255, 159, 64, 0.2)"
        ];
        const borderColors =  [
            "rgba(255, 99, 132, 1)",
            "rgba(54, 162, 235, 1)",
            "rgba(255, 206, 86, 1)",
            "rgba(75, 192, 192, 1)",
            "rgba(153, 102, 255, 1)",
            "rgba(255, 159, 64, 1)"
        ];
        // const messageCountSum = hostnameCount.reduce((sum, item) => sum + item.message_count, 0);
        const [data, labels, barBgColors, barBorderColors] = [[], [], [], []];
        for (let i = 0; i < hostnameCount.length; i++) {
            data.push(hostnameCount[i].message_count);
            labels.push(hostnameCount[i].host_name);
            barBgColors.push(backgroundColors[i % backgroundColors.length]);
            barBorderColors.push(borderColors[i % borderColors.length]);
        }

        const config = {
            type: 'bar',
            data: {
                labels,
                datasets: [{
                    data,
                    backgroundColor: barBgColors,
                    borderColor: barBorderColors,
                    borderWidth: 1,
                    barPercentage: 0.5
                }]
            },
            options: {
                scales: {
                    xAxes: [{
                        ticks: {
                            beginAtZero: true
                        },
                        scaleLabel: {
                            display: true,
                            labelString: 'Website name'
                        }
                    }],
                    yAxes: [{
                        ticks: {
                            beginAtZero: true
                        },
                        scaleLabel: {
                            display: true,
                            labelString: 'Number of visits'
                        }
                    }],
                },
                legend: {
                    display: false,
                },

            }
            // options: options hostnameCount.map(item => ({x: item.host_name, y: item.message_count}))
        };

        return (
            <Chart config={config}/>
        );
    };

    render() {
        const {allDataLoaded} = this.state;
        if (!allDataLoaded) {
            return <Spinner/>
        }
        const {error, profile } = this.state;

        return error ? <NotFound/> : (
            <div className="card">
                <div className="card-body">
                    <div className="row">
                        {this.renderProfile()}
                    </div>
                    <div id="employee-collected-data">
                        <hr/>
                        <div className="row">
                            <h5 className="d-block text-center">
                                <a id="screenshots-heading" ref={this.screenshotHeadingRef}>Recent screenshots</a>
                            </h5>
                            <div className="col-12">
                                {this.renderScreenshots()}
                            </div>
                        </div>
                        <hr/>
                        <div className="row">
                            <h5 className="d-block text-center">
                                <a id="domains-heading" ref={this.domainsHeadingRef}>Recent visited websites</a>
                            </h5>
                            <div className="col-12">
                                {this.renderDomains()}
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        );
    }
}
