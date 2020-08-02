import React from 'react';
import { useLocation } from 'react-router-dom';
import { Pagination, PaginationItem, PaginationLink } from "reactstrap";

function joinPaths(p1, p2) {
    if (p1.charAt(p1.length - 1) !== "/") {
        p1 += "/"
    }
    return p1 + p2;
}

export default class Paginator extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            page: props.page,
            count: props.count,
            step: props.step || 2
        };
        this.onPageChange = props.onPageChange;
    }

    static getDerivedStateFromProps(props, state) {
        if (props.page !== state.page) {
          return {
            page: props.page
          };
        }

        // Return null to indicate no change to state.
        return null;
    }

    generatePaginationRange() {
        let { page, count, step } = this.state;
        const range = [];
        for (let i = page - step; i < page + step; i++) {
            // console.log("iter", i);
            if (i <= 0) {
                step += 1;
                continue;
            }
            if (i > count) {
                break;
            }
            range.push(i);
        }
        return range;
    }

    render() {
        // const { pathname } = window.location;
        // console.log(location.pathname);
        const { page, count } = this.state;
        const range = this.generatePaginationRange();
        return (
            <Pagination className={this.props.className}>
                <PaginationItem disabled={page === 1}>
                    <PaginationLink first onClick={e => {e.preventDefault(); this.onPageChange(1); }}/>
                </PaginationItem>
                <PaginationItem disabled={page === 1}>
                    <PaginationLink previous onClick={e => this.onPageChange(page - 1)}/>
                </PaginationItem>
                {range.map(idx => (
                    <PaginationItem
                        active={idx === page}
                        key={idx}
                    >
                        <PaginationLink
                            // href={joinPaths(pathname, `?page=${idx}`)}
                            onClick={e => {e.preventDefault(); this.onPageChange(idx);}}
                        >
                            {idx}
                        </PaginationLink>
                    </PaginationItem>
                ))}
                <PaginationItem disabled={page === count}>
                    <PaginationLink next onClick={e => {e.preventDefault(); this.onPageChange(page + 1);}}/>
                </PaginationItem>
                <PaginationItem disabled={page === count}>
                    <PaginationLink last onClick={e => {e.preventDefault(); this.onPageChange(count);}}/>
                </PaginationItem>
            </Pagination>
        );
    }
}